add_rules("mode.debug", "mode.release")

add_requires("ncurses")

target("LChatServer")
set_kind("binary")
set_toolset("cc", "clang")
set_languages("c11")

add_includedirs("LChat/include", { public = true })
add_files("LChat/src/Core/Server/*.c", "LChat/src/Utils/*.c", "LChat/mainServer.c", { public = true })

add_packages("ncurses")

if is_mode("debug") then
	add_cflags("-Wall", "-Wextra", "-Wshadow", "-Wunused", "-Wno-gnu-zero-line-directive", { force = true })
	set_symbols("debug")
	set_optimize("none")
	add_defines("DEBUG")
end

if is_mode("release") then
	set_symbols("hidden")
	set_optimize("fastest")
	set_strip("all")
	add_defines("NDEBUG")
end

target("LChatClient")
set_kind("binary")
set_toolset("cc", "clang")
set_languages("c11")

add_includedirs("LChat/include", { public = true })
add_files("LChat/src/Core/Client/*.c", "LChat/src/Utils/*.c", "LChat/mainClient.c", { public = true })

add_packages("ncurses")

if is_mode("debug") then
	add_cflags("-Wall", "-Wextra", "-Wshadow", "-Wunused", "-Wno-gnu-zero-line-directive", { force = true })
	set_symbols("debug")
	set_optimize("none")
	add_defines("DEBUG")
end

if is_mode("release") then
	set_symbols("hidden")
	set_optimize("fastest")
	set_strip("all")
	add_defines("NDEBUG")
end
