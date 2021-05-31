% Automatically generated Matlab file for Lot Sizing Problem
%
% Instance size: 15 periods, 8 item types
%
% h - storage cost
% C - 8x8 matrix containing change-over costs between 8 different item types
% D - 15x8 matrix containing demand D(p,t) for period p in 1..15 and item type t in 1..8
h = 10;

C = [
 0  78  86  93  120  12  155  20 ;
 165  0  193  213  178  12  90  20 ;
 214  170  0  190  185  12  40  20 ;
 178  177  185  0  196  12  155  66 ;
 201  199  215  190  0  12  155  20 ;
 201  100  88  190  14  0  75  70 ;
 50  44  155  190  111  12  0  20 ;
 201  199  215  190  123  70  155  0 ;
];
D = [
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  0  0 ;
 0  0  0  0  0  0  1  0 ;
 0  0  0  0  0  0  0  0 ;
 1  0  0  0  0  0  0  0 ;
 0  1  0  0  1  0  0  1 ;
 0  0  1  0  1  1  0  0 ;
 0  0  0  0  0  0  0  1 ;
 0  1  0  0  0  0  0  0 ;
 0  0  0  1  0  0  0  0 ;
 0  0  0  0  0  1  0  0 ;
 0  0  0  0  0  0  0  0 ;
];
