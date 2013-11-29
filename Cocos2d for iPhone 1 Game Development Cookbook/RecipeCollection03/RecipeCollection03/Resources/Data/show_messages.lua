function restOfScript()
	scene.appendMessage("This is a Lua script.");
	script.waitSeconds(1);
	scene.appendMessage("Pausing script.");
	script.pause();
	scene.appendMessage("Script resumed.");
	return;
end
script.waitSeconds(1);
scene.appendMessage("Hello");
script.waitSeconds(1);
restOfScript();





