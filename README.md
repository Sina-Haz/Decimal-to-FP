# Decimal to Floating Point

This program takes in decimal and converts to a variable bit IEEE-FP representation with a ties-to-even rounding mode

_I/O format_:
The program takes in one file name as command-line input. Each line of the input file will have a decimal, the number of bits for the FP representation, the number of exponent bits as well as the number of fraction bits. For each line of input the program outputs the FP representation with the specified number of bits followed by a newline.

Example input:
```
6.5 8 4 3
.0546875 8 4 3
.013671875 8 4 3
6.375 8 4 3
8.5 8 4 3
9.5 8 4 3
```
Result:
```
$./convert input.text
01001101
00010110
00000111
01001101
01010000
01010010
```
