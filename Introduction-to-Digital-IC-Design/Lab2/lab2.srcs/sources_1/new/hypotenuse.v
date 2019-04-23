`timescale 1ns / 1ps

module hypotenuse(
    input clk,
    input rst,
    input start,
    
    input [7:0] a_in,
    input [7:0] b_in,
    
    output busy,
    output reg [15:0] out
);
    reg [3:0] state;
    
    localparam Idle = 3'b000;
    localparam BeginMul = 3'b001;
    localparam WaitMul = 3'b010;
    localparam BeginSqrt = 3'b011;
    localparam WaitSqrt = 3'b100;
    
    assign busy = state != Idle;
    
    reg sq1_rst, sq1_start, sq2_rst, sq2_start;
    reg [7:0] sq1_in, sq2_in;
    
    wire sq1_busy, sq2_busy;
    wire [15:0] sq1_out, sq2_out;
    
    mul sq1(
        .clk(clk), .rst(sq1_rst), .start(sq1_start),
        .a_in(sq1_in), .b_in(sq1_in), .busy(sq1_busy), .out(sq1_out)
    );
    
    mul sq2(
        .clk(clk), .rst(sq2_rst), .start(sq2_start),
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
            sq1_rst <= 1;
            sq2_rst <= 1;
            sqrt_rst <= 1;
        end
        else case (state)
            Idle:
                if (start) begin
                    state <= BeginMul;
                    sq1_in <= a_in;
                    sq2_in <= b_in;
                    sq1_rst <= 0;
                    sq2_rst <= 0;
                    sq1_start <= 1;
                    sq2_start <= 1;
                end
            BeginMul:
                state <= WaitMul;
            WaitMul:
                if (!sq1_busy && !sq2_busy) begin
                    state <= BeginSqrt;
                    sq_sum <= sq1_out + sq2_out;
                    sqrt_rst <= 0;
                    sqrt_start = 1;
                end
            BeginSqrt:
                state <= WaitSqrt;
            WaitSqrt:
                if (!sqrt_busy) begin
                    state <= Idle;
                    out <= sqrt_out;
                end
        endcase
endmodule
