function assert(a, b)
{
	if (a == b) {
		return '[OK]'
	} else {
		throw "Assertion failure: " + a + " != " + b
	}
}

function iterate(obj) {
    for (var i in obj) {
        if(obj[i] instanceof Array) {
			console.log()
            iterate(obj[i])
        } else if(obj[i] instanceof Object){
            console.log("Object: " + i + ': name = ' + obj[i].name() + ', value = ' + obj[i].value() + ', class = ' + obj[i].class() + ', description = ' + obj[i].description())
            iterate(obj[i].uiElements())
        }
    }
}

function BaseTestCase() {
    this.currentApplication = Application.currentApplication()
    this.currentApplication.includeStandardAdditions = true
    this.systemEvents = Application("System Events")
}

BaseTestCase.prototype.runShellCmd = function(cmd) {
    return this.currentApplication.doShellScript(cmd)
}

BaseTestCase.prototype.isAppRunning = function(appname) {
    var procList = this.systemEvents.processes()

    for (var i=0; i < procList.length; ++i) {
        if (procList[i].name() == appname) {
            return true
        }
    }
    return false
}

BaseTestCase.prototype.getAppProcess = function(appname) {
    return this.systemEvents.processes.byName(appname)
}

BaseTestCase.prototype.runApp = function(bundle_path, appname, wait) {
    this.runShellCmd("open " + bundle_path)
    delay(wait)
    app = Application(appname)
    app.activate()
}

BaseTestCase.prototype.pressKeyCode = function(kk) {
    this.systemEvents.keyCode(kk)
}

BaseTestCase.prototype.sendKeyCombination = function(modifiers, key) {
    this.systemEvents.keystroke(key, {using: modifiers})
}

BaseTestCase.prototype.quitByHotkey = function() {
    this.sendKeyCombination(['command down'], 'q')
}

BaseTestCase.prototype.quitApp = function(appname = '') {
    if (appname != '') {
        an = appname
    } else {
        an = this.appname
    }
    app = Application(an)
    app.quit()
}

BaseTestCase.prototype.typeString = function(str) {
    var customKeyCodes = {
        "0":29,
        "1":18,
	    "2":19,
	    "3":20,
	    "4":21,
	    "5":23,
	    "6":22,
	    "7":26,
	    "8":28,
	    "9":25,
	    "-":27
    }
    for(var i=0; i < str.length; ++i) {
	    var chr = str.charAt(i)
	    var charID = str.charCodeAt(i)

	    if (chr in customKeyCodes) {
		    this.systemEvents.keyCode(customKeyCodes[chr])
	    } else {
		    this.systemEvents.keystroke(chr)
	    }
    }
}

BaseTestCase.prototype.showAllElements = function(appname) {
    appProc = this.systemEvents.processes.byName(appname)
    iterate(appProc.windows())
}

BaseTestCase.prototype.clickButton = function(winID,btnID,appname) {
    var appProc = this.systemEvents.processes.byName(appname)
    appProc.windows[winID].buttons[btnID].click()
}

BaseTestCase.prototype.cleanUp = function() {
    if (this.isAppRunning(this.appname)) {
        this.runApp(this.bundle_path, this.appname, 1)
        this.quitByHotkey()
    }
}

BaseTestCase.prototype.runTests = function() {
    var test_methods = Object.keys(this).filter(key => key.startsWith('test_'))
    err_count = 0
    for (var i=0; i<test_methods.length; ++i) {
        try {
            st = "[OK]"
            console.log(this.constructor.name, test_methods[i])
            this[test_methods[i]]()
        } catch(e) {
            st = "[FAIL]"
            ++err_count
            console.log(e)
        } finally {
            console.log(this.constructor.name, test_methods[i], st)
            this.cleanUp()
        }
        if (err_count > 0 ) {
            throw "Some tests failed"
        }
    }
}
