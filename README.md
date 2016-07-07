# luajit-c-embedding

Here are some notes/instructions for embedding the LuaJIT in C. Initially, we follow along with [Marek Vavrusa's blog post](https://en.blog.nic.cz/2015/08/12/embedding-luajit-in-30-minutes-or-so/). Later, we move beyond and see how to interact with things in Torch.

## Hello, World!

Let's start by creating a Lua script named hello.lua which contains ```print('Hello, World!')```. Next, create a C file hello-luajit.c which looks like this (modified from the above blog post):

```
int main(int argc, char *argv[])
{
	int status;
	lua_State *L;
	
	L = luaL_newstate(); // open Lua
    if (!L) {
		return -1; // Checks that Lua started up
	}
	
	luaL_openlibs(L); // load Lua libraries
		
	if (argc > 1) {
		status = luaL_loadfile(L, argv[1]);  // load Lua script
        int ret = lua_pcall(L, 0, 0, 0); // tell Lua to run the script
		if (ret != 0) {
			fprintf(stderr, "%s\n", lua_tostring(L, -1));
			return 1;
		}
	}
	
	lua_close(L); // Close Lua
	return 0;
}
```

To compile the C, use:

```gcc hello-luajit.c $(pkg-config --cflags --libs luajit) -lm -ldl -pagezero_size 10000 -image_base 100000000 -o hello.out```, noting that ```-pagezero_size 10000 -image_base 100000000``` only needs to be included on macOS systems.

You can now execute the Lua script with ```./hello.out hello.lua```.

### Understanding the C Code

To interact with Lua from C we will use [Lua's C API](http://www.lua.org/pil/24.1.html). Let's look at the each part of hello-luajit.c.

- ```luaL_newstate```: Starts up Lua and returns a new Lua state
- ```luaL_openlibs```: loads the Lua standard libraries
- ```luaL_loadfile```: Loads the Lua script
- ```lua_pcall```: runs the Lua script and returns an error code
- ```lua_close```: closes Lua

