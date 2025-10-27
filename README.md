Advent of Code 2015 solved in C with minimum dependencies.

Build and run using [just](https://just.systems/man/en/):

```
$ just build
$ just run <day-number>
```

### Credits
- Unit testing: https://github.com/mity/acutest
- md5: https://github.com/Zunawe/md5-c

## Writeup

### Day 1

Basic parsing problem, solvable in a few lines of pure C, nothing special here.

### Day 2

Slightly more complex parsing, solvable by `sscanf()`. The solution required
sorting three integers, could have been done by hand, I did it using `qsort()`
just for the heck of it.

### Day 3
First major hurdle if you want to rely only on the standard libc. This one required associative array, so I implemented dynamic array and a hashtable backed by the dynamic array. These are toy data structures for the moment, heavily inspired by Sean Barrett's [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h). For the moment they don't support element deletion, nor does the hashtable support keys of varied length (e.g. strings).

### Day 4

This one required calculating MD5 hashing, not something I would do by hand, so I grabbed a ready-made solution from Github, thanks, @Zunawe!

### Day 5

Hashtable for the rescue again (although this one probably could have been done even faster with a dict backed by a list, since the data is so short).

### Day 6

~~This one called for a bitset, which I had to implement from scratch (I know there is much room for preformance improvement in my half-assed implementation, but it does the job).~~

Turns out bitset is useless for part two, so I rewrote it to use a regular 2d array of bytes.

### Day 7

Had to implement a hashtable with strings as keys, as well as an arena allocator. The whole thing with reimplementation of primitive data structures is becoming a bit tedious to be honest, maybe I'll switch to third-party libraries soon (or not).

### Day 8

This problem is just silly.

### Day 9

Traveling salesman for a small number of nodes calls for simple data structures, like lookups in an array.
