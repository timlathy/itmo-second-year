`timescale 1ns / 1ps

module fsm(
    input rst,
    input clk,
    input [1:0] a,
    output reg [3:0] y
);
    reg [1:0] state;
    reg [1:0] next_state;
    
    localparam S0 = 2'b00;
    localparam S1 = 2'b01;
    localparam S2 = 2'b10;
    localparam S3 = 2'b11;
    
    always@(posedge clk) begin
        if (rst) begin
            state = S0;
            next_state = S0;
        end
        else begin
            state = next_state;
            case (state)
                S0:
                    if (a[1])
                        next_state = S1;
                S1:
                    if (a[0])
                        next_state = S0;
                    else if (!a[1])
                        next_state = S2;
                S2:
                    if (a[0])
                        next_state = S3;
                S3:
                    if (!a[0])
                        next_state = S1;
                    else if (a[0] && a[1])
                        next_state = S0;
                default:
                    next_state = state;
            endcase
        end
    end
    
    always@(posedge clk)
        case (state)
            S0: y = 4'b0001;
            S1: y = 4'b0010;
            S2: y = 4'b0100;
            S3: y = 4'b1000;
        endcase
endmodule
