`timescale 1ns / 1ps

module majority_circuit_testbench;
    reg a_in, b_in, c_in, d_in, e_in;
    wire mc_out;
    
    majority_circuit mc(
        .a(a_in), .b(b_in), .c(c_in), .d(d_in), .e(e_in), .out(mc_out)
    );
    
    reg[5:0] test;
    integer test_bits_set;
    reg test_expected;
    
    initial begin
        for (test = 0; test < 32; test = test + 1) begin
            a_in = test[0];
            b_in = test[1];
            c_in = test[2];
            d_in = test[3];
            e_in = test[4];
            test_bits_set = test[0] + test[1] + test[2] + test[3] + test[4];
            test_expected = test_bits_set >= 3;
            
            #1 // wait 1ns
            
            if (mc_out == test_expected) begin
                $display("The output is correct for %b", test);
            end
            else begin
                $display("The output is incorrect! Expected %b for %b", test_expected, test);
                $stop;
            end
        end
    end
endmodule
