`timescale 1ns / 1ps

module hypotenuse(
    input clk,
    input rst,
    input [15:0] sw,

    output busy,
    output reg [15:0] leds
);
    reg [3:0] state;
    reg [15:0] current_sw;
    wire [7:0] a_in, b_in;
    wire start;
    
    assign a_in = current_sw[7:0];
    assign b_in = current_sw[15:8];
    
    localparam Idle = 3'b000;
    localparam Start = 3'b001;
    localparam BeginSquare = 3'b010;
    localparam WaitSquare = 3'b011;
    localparam BeginSqrt = 3'b100;
    localparam WaitSqrt = 3'b101;
    
    assign start = sw != current_sw;
    assign busy = state != Idle;
    
    reg sq12_rst, sq12_start;
    reg [7:0] sq1_in, sq2_in;
    
    wire sq1_busy, sq2_busy;
    wire [15:0] sq1_out, sq2_out;
    
    mul sq1(
        .clk(clk), .rst(sq12_rst), .start(sq12_start),
        .a_in(sq1_in), .b_in(sq1_in), .busy(sq1_busy), .out(sq1_out)
    );
    
    mul sq2(
        .clk(clk), .rst(sq12_rst), .start(sq12_start),
        .a_in(sq2_in), .b_in(sq2_in), .busy(sq2_busy), .out(sq2_out)
    );
    
    reg [15:0] sq_sum;
    reg sqrt_rst, sqrt_start;
    wire [15:0] sqrt_out;
    wire sqrt_busy;
    
    sqrt sqrt1(
        .clk(clk), .rst(sqrt_rst), .start(sqrt_start),
        .x_in(sq_sum), .busy(sqrt_busy), .out(sqrt_out)
    );
    
    always@(posedge clk)
        if (rst) begin
            state <= Idle;
            leds <= 0;
            current_sw <= 0;
        end
        else case (state)
            Idle:
                if (start) begin
                    state <= Start;
                    sq12_rst <= 1;
                    sqrt_rst <= 1;
                    leds <= 0;
                    current_sw <= sw;
                end
            Start: 
                begin
                    state <= BeginSquare;
                    sq1_in <= a_in;
                    sq2_in <= b_in;
                    sq12_rst <= 0;
                    sq12_start <= 1;
                end
            BeginSquare:
                state <= WaitSquare;
            WaitSquare:
                if (!sq1_busy && !sq2_busy) begin
                    state <= BeginSqrt;
                    sq_sum <= sq1_out + sq2_out;
                    sqrt_rst <= 0;
                    sqrt_start <= 1;
                end
            BeginSqrt:
                state <= WaitSqrt;
            WaitSqrt:
                if (!sqrt_busy) begin
                    state <= Idle;
                    leds <= sqrt_out;
                end
        endcase
endmodule
