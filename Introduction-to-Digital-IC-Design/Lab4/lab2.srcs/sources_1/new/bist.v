`timescale 1ns / 1ps

module bist(
    input clk,
    input rst,
    input toggle_test,
    input [15:0] switches,
    output test_running,
    output reg [15:0] leds
);
    reg [3:0] state;
    reg [7:0] total_test_runs, test_iter;
    
    reg [7:0] hyp_a, hyp_b;
    reg hyp_rst, hyp_start;
    wire hyp_busy;
    wire [7:0] hyp_out;
    
    wire test_state;
    btn_debouncer btn_toggle_test(
        .clk(clk), .btn(toggle_test), .btn_state(test_state)
    );
    
    hypotenuse hyp(
        .clk(clk), .rst(hyp_rst), .a_in(hyp_a), .b_in(hyp_b), .start(hyp_start),
        .busy(hyp_busy), .out(hyp_out));
    
    wire [7:0] lfsr_a_out, lfsr_b_out;
    reg lfsr_rst;
    lfsr #(.Polynomial(8'b00101101)) lfsr_a(.clk(clk), .rst(lfsr_rst), .out(lfsr_a_out));
    lfsr #(.Polynomial(8'b00101101)) lfsr_b(.clk(clk), .rst(lfsr_rst), .out(lfsr_b_out));
    
    wire [7:0] crc_out;
    reg crc_rst, crc_start;
    wire crc_busy;
    crc8 crc8(.clk(clk), .rst(crc_rst), .start(crc_start), .val(hyp_out), .data(crc_out), .busy(crc_busy));
    
    localparam Idle = 4'b0000;
    localparam BeginPassthrough = 4'b0001;
    localparam WaitPassthrough = 4'b0010;
    localparam StartTestIter = 4'b0011;
    localparam PrepareTest = 4'b0100;
    localparam BeginTest = 4'b0101;
    localparam WaitTest = 4'b0110;
    localparam BeginCrc = 4'b0111;
    localparam WaitCrc = 4'b1000;
    localparam TestFinished = 4'b1001;
    localparam TestButtonUnpressed = 4'b1010;
    localparam TestButtonPressed = 4'b1011;
    
    assign test_running =
        state == StartTestIter ||
        state == PrepareTest ||
        state == BeginTest ||
        state == WaitTest ||
        state == BeginCrc ||
        state == WaitCrc ||
        state == TestFinished ||
        state == TestButtonUnpressed ||
        state == TestButtonPressed;
    
    always@(posedge clk)
        if (rst) begin
            state <= Idle;
            total_test_runs <= 0;
            hyp_start <= 0;
            hyp_rst <= 1;
            leds <= 0;
            hyp_a <= 0;
            hyp_b <= 0;
        end
        else case (state)
            Idle:
                if (test_state) begin
                    state <= StartTestIter;
                    test_iter <= 0;
                    crc_rst <= 1;
                    lfsr_rst <= 1;
                    crc_start <= 0;
                end
                else if (hyp_a != switches[7:0] || hyp_b != switches[15:8]) begin
                    state <= BeginPassthrough;
                    hyp_a <= switches[7:0];
                    hyp_b <= switches[15:8];
                    hyp_rst <= 0;
                    hyp_start <= 1;
                end
             BeginPassthrough: begin
                state <= WaitPassthrough;
                hyp_start <= 0;
             end
             WaitPassthrough:
                if (!hyp_busy) begin
                    state <= Idle;
                    leds <= hyp_out;
                end
            StartTestIter: begin
                state <= PrepareTest;
                lfsr_rst <= 0;
                crc_rst <= 0;
                hyp_rst <= 1;
            end
            PrepareTest: begin
                state <= BeginTest;
                hyp_a <= lfsr_a_out;
                hyp_b <= lfsr_b_out;
                hyp_rst <= 0;
                hyp_start <= 1;
            end
            BeginTest: begin
                state <= WaitTest;
                hyp_start <= 0;
            end
            WaitTest:
                if (!hyp_busy) begin
                    state <= BeginCrc;
                    crc_start <= 1;
                 end
            BeginCrc: begin
                crc_start <= 0;
                state <= WaitCrc;
            end
            WaitCrc:
                if (!crc_busy) begin
                    leds[15:8] <= crc_out;
                    if (test_iter == 255) begin
                        state <= TestFinished;
                        total_test_runs <= total_test_runs + 1;
                        leds[7:0] <= total_test_runs + 1;
                    end
                    else begin
                        state <= StartTestIter;
                        test_iter <= test_iter + 1;
                    end
                end
            TestFinished:
                if (!test_state)
                    state <= TestButtonUnpressed;
            TestButtonUnpressed:
                if (test_state)
                    state <= TestButtonPressed;           
            TestButtonPressed:
                if (!test_state) begin
                    state <= Idle;
                    hyp_rst <= 1;
                    leds <= 0;
                    hyp_a <= 0;
                    hyp_b <= 0;
                end
        endcase
endmodule
