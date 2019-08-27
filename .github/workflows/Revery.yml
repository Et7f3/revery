on: [push, pull_request]

jobs:
  build:
    name: Revery CI
    
    runs-on: ${{ matrix.os }}
    
    strategy:
      matrix:
        # macOS-latest fail with -lpng not found
        # ubuntu-latest fail with ragel not found
        # os: [ubuntu-latest, windows-latest, macOS-latest]
        os: [windows-latest]
        node-version: [10.x]

    steps:
    - uses: actions/checkout@v1
    - name: Use Node.js ${{ matrix.node-version }} on ${{ matrix.os}}
      uses: actions/setup-node@v1
      with:
        node-version: ${{ matrix.node-version }}
    - name: install esy
      run: |
        npm install -g esy
    - name: Install dependencies
      run: |
        esy install
    - name: Building all in Revery
      run: |
        esy build dune build @all
    - name: Building Revery
      run: |
        esy build
      env:
        CI: true
