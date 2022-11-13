# ATOIP Converter

```c
Usage: ./atoip [ -r ] [ -p ] [ -e ] STRING [ STRING... ]
```

## Summary

This program takes command line strings and outputs equivalent int arrays.

Each `STRING` is converted into an `int[]` array with ints corresponding to the chars in the string

Additional `STRING` arguments will output more arrays. To keep everything in a single array, ensure it is a single argument

## Options

The `-r` option reverses the endianness of the ints output in the array. By default it uses x86 Little-Endian output

The `-p` option pads final ints to have the same length as all others. Without the `-p` option the final int in the array will only be as long as needed, but with the `-p` option it will have additional 0s to be as long as all other ints

The `-e` option allows extended mode. By default you can only output $676$ arrays ($26*26$) but with the `-e` option you can have up to $208,827,064,576$ arrays ($26^8$);
