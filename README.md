# Extendible-Hashing
Extendible Hashing in C

### How To Compile
    gcc hashing.c -lm
### How TO Run
    ./<executable_file> <initial_directory_size> <bucket_size> <delete_option> <input_file_name>

### Details For Arguments
##### Initial Directory Size
These many directory entries will be there in the hash table initially.
##### Bucket Size
A bucket can contain these many entries in it.
##### Delete Option
1. LazyDelete
    With this option calling delete <key> will delete the entry from corosponding bucket.
2. MergeBucketDelete
    With this option calling delete <key> wil delete the entry from corosponding bucket as well as if possible(if bucket gets enmpty) the bucket will be merged with its mirror bucket.
3 ShrinkDirectoryDelete
    With this option calling delete <key> will do what MergeBucketDelete do. In addition, it will shrink the directry structure.

###### Condition For Delete Option 2 and 3
 1. Bucket merge condition
– Local Depths of buckets patricipating in the merge are the same
– Last (local_depth - 1) bits of the hash key are the same
 2. Directory shrink condition
– Local Depths of all buckets are smaller than the Globle Depth