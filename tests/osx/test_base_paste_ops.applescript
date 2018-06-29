function SKBasePasteOps() {
    BaseTestCase.call(this)
    this.bundle_path = this.runShellCmd("echo $BUNDLE_PATH")
    this.appname = 'silverkey-qt'
    this.terminal_bundle = '/Applications/Utilities/Terminal.app'
    this.terminal_app = 'Terminal'

    this.test_open_close = function() {
        this.runApp(this.bundle_path, this.appname, 1)
        assert(this.isAppRunning(this.appname), true)

        this.quitApp()
        delay(1)
        assert(this.isAppRunning(this.appname), false)
    }

    this.test_open_esc_close = function() {
        this.runApp(this.bundle_path, this.appname, 1)
        assert(this.isAppRunning(this.appname), true)

        //Press Esc key
        this.pressKeyCode(53)
        assert(this.isAppRunning(this.appname), true)

        this.runApp(this.bundle_path, this.appname, 1)
        this.quitApp()
        delay(1)
        assert(this.isAppRunning(this.appname), false)
    }

    this._default_paste_flow = function(key, result) {
        returnKeyCode = 36
        this.runApp(this.terminal_bundle, this.terminal_app, 2)
        this.typeString('TEST_STRING=')

        this.runApp(this.bundle_path, this.appname, 1)

        this.typeString(key)
        delay(5)
	    //Press return
	    this.pressKeyCode(returnKeyCode)
	    delay(1)

	    //Focus shold be in terminal so press return
	    assert(this.getAppProcess('Terminal').frontmost(), true)
        this.pressKeyCode(returnKeyCode)
        delay(1)

	    timeStamp = this.runShellCmd("date +%s")
	    this.typeString("echo -n $TEST_STRING > /tmp/test" + timeStamp)
	    delay(1)
	    this.pressKeyCode(returnKeyCode)
	    delay(1)

        this.runApp(this.terminal_bundle, this.terminal_app, 2)
        this.quitApp(this.terminal_app)
        delay(2)

	    res = this.runShellCmd("cat /tmp/test" + timeStamp)
	    assert(res, result)
    }

    this.test_open_paste_full = function() {
        fullKey = "/chemistry/gold/boil"
        expected_result = "3243"
        this._default_paste_flow(fullKey, expected_result)
    }

    this.test_open_paste_fuzzy = function() {
        fuzzyKey = "che go bo"
        expected_result = "3243"
        this._default_paste_flow(fuzzyKey, expected_result)
    }
}

SKBasePasteOps.prototype = Object.create(BaseTestCase.prototype);
SKBasePasteOps.prototype.constructor = SKBasePasteOps;

t = new SKBasePasteOps()
t.runTests()
