`timescale 1ns / 1ps

module mul_testbench;
    reg clk_in, rst_in, start_in;
    reg [7:0] a_in, b_in;
    wire busy_out;
    wire [15:0] mul_out;
    
    mul m(
        .clk(clk_in), .rst(rst_in), .start(start_in), .a_in(a_in), .b_in(b_in),
        .busy(busy_out), .out(mul_out)
    );
    
    initial begin
        clk_in = 0;
        forever #8 clk_in = !clk_in; // alternate clock every 8ns
    end
    
    integer expected;
    
    initial begin
        for (a_in = 0; a_in < 255; a_in = a_in + 1) begin
            for (b_in = 0; b_in < 255; b_in = b_in + 1) begin
                expected = a_in * b_in;
                
                repeat(1) @(posedge clk_in);
                rst_in = 1'b1;
                
                repeat(1) @(posedge clk_in);
                rst_in = 1'b0;
                start_in = 1'b1;
                
                repeat(1) @(posedge clk_in);
                
                while (busy_out) begin
                    repeat (1) @(posedge clk_in);
                end
                
                if (mul_out == expected)
                    $display("[+] %d * %d = %d", a_in, b_in, mul_out);
                else begin
                    $display("Expected %d for %d * %d, got %d", expected, a_in, b_in, mul_out);
                    $stop;
                end
            end
        end
        $finish;
    end
endmodule
