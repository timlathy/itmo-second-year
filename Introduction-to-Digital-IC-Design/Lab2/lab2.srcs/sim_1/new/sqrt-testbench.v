`timescale 1ns / 1ps

module sqrt_testbench;
    parameter CLOCK_HALF_PERIOD = 500; // 1000ns = 100MHz
    
    reg clk_in, rst_in, start_in;
    reg [15:0] x_in;
    wire busy_out;
    wire [15:0] sqrt_out;
    
    sqrt s(
        .clk(clk_in), .rst(rst_in), .start(start_in), .x_in(x_in),
        .busy(busy_out), .out(sqrt_out)
    );
    
    initial begin
        clk_in = 0;
        forever #CLOCK_HALF_PERIOD clk_in = !clk_in;
    end
    
    integer x_sqrt;
    
    initial begin
        for (x_sqrt = 0; x_sqrt < 255; x_sqrt = x_sqrt + 1) begin
            for (x_in = x_sqrt * x_sqrt; x_in < (x_sqrt + 1) * (x_sqrt + 1); x_in = x_in + 1) begin
                repeat(1) @(posedge clk_in);
                rst_in = 1'b1;
                
                repeat(1) @(posedge clk_in);
                rst_in = 1'b0;
                start_in = 1'b1;
                
                repeat(1) @(posedge clk_in);
                
                while (busy_out)
                    repeat (1) @(posedge clk_in);
                    
                if (sqrt_out == x_sqrt)
                    $display("[+] sqrt(%d) = %d", x_in, sqrt_out);
                else begin
                    $display("Expected %d for sqrt(%d), got %d", x_sqrt, x_in, sqrt_out);
                    $stop;
                end
            end
        end
        $finish;
    end
endmodule
