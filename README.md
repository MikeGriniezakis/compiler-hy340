Sure, here's the updated README without the git cloning step:

# Building the CMake Project

This guide will help you build the CMake project.

## Prerequisites

- CMake version 3.28 or higher
- Flex library

## Steps

1. **Navigate to the project directory**

   Use the `cd` command to navigate into the project directory.

   ```bash
   cd hy340
   ```

2. **Create a build directory**

   It's a good practice to create a separate directory for the build files. You can do this with the following command:

   ```bash
   mkdir build
   ```

3. **Navigate to the build directory**

   Use the `cd` command to navigate into the build directory.

   ```bash
   cd build
   ```

4. **Run CMake**

   Now you can run CMake to generate the build files. Use the following command:

   ```bash
   cmake ..
   ```

   The `..` tells CMake to look for a `CMakeLists.txt` file in the parent directory.

5. **Build the project**

   Finally, you can build the project with the following command:

   ```bash
   make
   ```

   This will build the project and create an executable file in the build directory.

## Running the Project

After building the project, you can run it with the following command:

```bash
./hy340 <path_to_input_file>
```

This will run the `hy340` executable. Replace `<path_to_input_file>` with the path to your input file.
