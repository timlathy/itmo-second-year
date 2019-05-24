    `timescale 1ns / 1ps
    
    module lfsr(
        input clk,
        input rst,
        output reg [7:0] out
    );
        parameter Polynomial = 8'b00000000;
        parameter InitData = 8'b01; // any non-zero value
    
        always@(posedge clk)
            if (rst)
                out <= InitData;
            else begin
                if (out[7])
                    out <= (out ^ Polynomial) << 1;
                else
                    out <= out << 1;
                out[0] <= out[7];
            end
    endmodule
