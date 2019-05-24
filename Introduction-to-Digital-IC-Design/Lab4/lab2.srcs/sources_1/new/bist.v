`timescale 1ns / 1ps

module bist(
    input clk,
    input rst,
    input toggle_test,
    input [15:0] switches,
    output test_running,
    output reg [15:0] leds
);
    reg [2:0] state;
    reg [7:0] total_test_runs, test_iter;
    
    reg [7:0] hyp_a, hyp_b;
    reg hyp_rst, hyp_start;
    wire hyp_busy;
    wire [7:0] hyp_out;
    
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
    localparam PrepareTest = 4'b0011;
    localparam WaitResetTest = 4'b0100;
    localparam BeginTest = 4'b0101;
    localparam WaitTest = 4'b0110;
    localparam BeginCrc = 4'b0111;
    localparam WaitCrc = 4'b1000;
    
    assign test_running =
        state == PrepareTest ||
        state == WaitResetTest ||
        state == BeginTest ||
        state == WaitTest ||
        state == BeginCrc ||
        state == WaitCrc;
    
    always@(posedge clk)
        if (rst) begin
            state <= Idle;
            total_test_runs <= 0;
            hyp_rst <= 1;
            hyp_a <= 0;
            hyp_b <= 0;
        end
        else case (state)
            Idle:
                if (toggle_test) begin
                    state <= PrepareTest;
                    test_iter <= 0;
                    crc_rst <= 1;
                    lfsr_rst <= 1;
                    crc_start <= 0;
                end
                else if (hyp_a != switches[7:0] && hyp_b != switches[15:8]) begin
                    state <= BeginPassthrough;
                    hyp_a <= switches[7:0];
                    hyp_b <= switches[15:8];
                    hyp_rst <= 0;
                    hyp_start <= 1;
                end
             BeginPassthrough:
                state <= WaitPassthrough;
             WaitPassthrough:
                if (!hyp_busy) begin
                    state <= Idle;
                    leds <= hyp_out;
                end
            PrepareTest: begin
                state <= WaitResetTest;
                hyp_a <= lfsr_a_out;
                hyp_b <= lfsr_b_out;
                lfsr_rst <= 0;
                crc_rst <= 0;
                hyp_rst <= 1;
            end
            WaitResetTest: begin
                state <= BeginTest;
                hyp_rst <= 0;
                hyp_start <= 1;
            end
            BeginTest:
                state <= WaitTest;
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
                        state <= Idle;
                        total_test_runs <= total_test_runs + 1;
                        leds[7:0] <= total_test_runs + 1;
                    end
                    else begin
                        state <= PrepareTest;
                        test_iter <= test_iter + 1;
                    end
                end
        endcase
endmodule
