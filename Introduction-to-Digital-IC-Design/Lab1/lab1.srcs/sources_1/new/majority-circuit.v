`timescale 1ns / 1ps

// http://www.32x8.com/circuits5_____A-B-C-D-E_____m_7-11-13-14-15-19-21-22-23-25-26-27-28-29-30-31___________option-0_____999783966371894585735

module majority_circuit(
    input a,
    input b, 
    input c,
    input d,
    input e,
    output out
);
    wire nor_abc, nor_abd, nor_abe, nor_acd, nor_ace, nor_ade,
        nor_bcd, nor_bce, nor_bde, nor_cde;
    
    nor(nor_abc, a, b, c);
    nor(nor_abd, a, b, d);
    nor(nor_abe, a, b, e);
    nor(nor_acd, a, c, d);
    nor(nor_ace, a, c, e);
    nor(nor_ade, a, d, e);
    nor(nor_bcd, b, c, d);
    nor(nor_bce, b, c, e);
    nor(nor_bde, b, d, e);
    nor(nor_cde, c, d, e);
    
    nor(out, nor_abc, nor_abd, nor_abe, nor_acd, nor_ace, nor_ade,
        nor_bcd, nor_bce, nor_bde, nor_cde);
endmodule
