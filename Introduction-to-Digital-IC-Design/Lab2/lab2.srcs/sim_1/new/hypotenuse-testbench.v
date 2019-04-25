`timescale 1ns / 1ps

module hypotenuse_testbench;
    parameter CLOCK_HALF_PERIOD = 500; // 1000ns = 100MHz

    reg clk_in, rst_in, start_in;
    reg [7:0] a_in, b_in;
    wire busy_out;
    wire [15:0] hyp_out;
    
    hypotenuse hyp(
        .clk(clk_in), .rst(rst_in), .start(start_in), .a_in(a_in), .b_in(b_in),
        .busy(busy_out), .out(hyp_out)
    );

    initial begin
        clk_in = 0;
        forever #CLOCK_HALF_PERIOD clk_in = !clk_in;
    end
    
    integer test_i, test_expected, cycles_taken;
    
    localparam TestCount = 4;
    
    integer a_test [0:(TestCount - 1)];
    integer b_test [0:(TestCount - 1)];
    integer out_test [0:(TestCount - 1)];
    
    initial begin
        a_test[0] = 4; b_test[0] = 7; out_test[0] = 8;
        a_test[1] = 44; b_test[1] = 24; out_test[1] = 50;
        a_test[2] = 123; b_test[2] = 79; out_test[2] = 146;
        a_test[3] = 181; b_test[3] = 181; out_test[3] = 255;
        
        for (test_i = 0; test_i < TestCount; test_i = test_i + 1) begin
            rst_in = 1;
            a_in = a_test[test_i];
            b_in = b_test[test_i];
            test_expected = out_test[test_i];
    
            repeat(2) @(posedge clk_in);
            
            rst_in = 0;
            start_in = 1;
            repeat(1) @(posedge clk_in);
            
            cycles_taken = 0;
            while (busy_out) begin
                repeat (1) @(posedge clk_in);
                cycles_taken <= cycles_taken + 1;
            end
            
            if (hyp_out == test_expected)
                $display("[+] sqrt(%d^2 + %d^2) = %d, cycles taken: %d", a_in, b_in, hyp_out, cycles_taken);
            else begin
                $display("Expected %d for sqrt(%d^2 + %d^2), got %d", test_expected, a_in, b_in, hyp_out);
                $stop;
            end
        end
        $finish;
    end
endmodule
