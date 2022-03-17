add_rules("mode.debug", "mode.release")

target("pilang")
	set_kind("binary")
	add_files("src/*.c")
	add_includedirs("src")

	if is_mode("debug") then
        add_defines("DEBUG")
    end
