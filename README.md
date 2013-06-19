Relative-Deployment
===================

A configurable windows process control tool. Designed to run web applications as if they were desktop applications. There will be much more to come here are a few missing features I still need to develop:

- [ ] Make a json command property on destruction, i.e if a command dies, does it kill the rest of the applications running along with it?
- [ ] Support console-less mode. Easy but then if you run command line applications they each get their own window, very annoying
- [ ] Find a why to hind console windows, maybe using services? Depends on previous issue.
- [ ] Auto restart if process dies. Stop reying after X attemps, how long to wait before considering an application to be successful


**Example Configuration File**

```json
{
  "commands": [
		{
			"name": "Php",
			"type": "process", 
			"port": "random",
			"cmd": "application\\php\\php.exe application\\src\\server.php $port"
		},
		{
			"name": "Browser",
			"type": "process", 
			"port": ":Php",
			"cmd": "browser\\app.exe -jsconsole -app browser\\application.ini -url http://localhost:$port/src/app.php"
		}
	]
}
```

Finding an Open Port
--------------------

The hard part to deploying a webapp to a desktop computer and knowing what port is open. Using **"port": "random"** will find a port open these ports are in the high 40,000+ numbers as it is standard that those ports not be used for traditional applications such as email, web server, etc. Once a port is picked it will check if it is open. If it is used it will choose another until one is found.

Using a Port From an Existing Command
-------------------------------------

If you used a random port and need that port to talk to the previous application you can reference the port but using **":"** for example if you named the previous command with **"name": "Php"** then the port should be **"port": ":Php"**
