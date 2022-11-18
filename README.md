# Shamir Secret Share 

This program will take some data as an ASCII string and split it
up into N pieces, M <= N of which can be used to reconstruct the 
original string. The pieces are provided in base 58 with a 
Bitcoin-style checksum. You can safely make up to one recording 
error per piece and the secret string will still be recoverable.

## Usage

### Split 

```
./SecretShare split <secret: string> <num_pieces: uint> <threshold: uint>
```

### Merge

```
./SecretShare merge <threshold: uint> <pieces: string...>
```

## Build

```
mkdir build
cd build
conan install ..
conan build
```

Binary file will be in `build/bin`. 
