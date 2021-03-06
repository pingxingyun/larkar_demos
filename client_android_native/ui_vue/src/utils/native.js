const JSBridge = window['$Native'];

export default class Native {
    static AR_SDK_TYPE_ARCORE = 1;
    static AR_SDK_TYPE_HW_ARENGINE = 2;

    static QuickConfigLevel_Fast      = 2;
    static QuickConfigLevel_Normal    = 3;
    static QuickConfigLevel_Extreme   = 4; 

    static showToast(toast) {
        JSBridge?.showToast(toast);
    }

    // enter appli to ar/vr activity
    // @param appid
    // @param isVR isVR applitype jump to vr acvitiy or ar activity
    static enterAppli(appid, isVR) {
        JSBridge?.enterAppli(appid, isVR);
    }

    static onSaveServerAddress(ip, port) {
        console.log('native onSaveServerAddress ', ip, parseInt(port));
        JSBridge?.onSaveServerAddress(ip, parseInt(port));
    }

    static hasPermission() {
        if (!JSBridge) {
            return false;
        }
        return JSBridge?.hasPermission();
    }

    static checkPermission() {
        console.log('native checkPermission');
        if (!JSBridge) {
            return false;
        }
        return JSBridge?.checkPermission();
    }

    /**
     * arcoreSupportStatus
     * @returns 0 support 1 need check 2 unsupport -1 env notready
     */
    static arcoreSupportStatus() {
        console.log('native arcoreSupportStatus');
        if (!JSBridge) {
            return -1;
        }
        return JSBridge?.arcoreSupportStatus();
    }

    /**
     * @param require need install
     * @returns 0 support 1 need install 2 error -1 env notready
     */
    static arcoreInstallStatus(require) {
        console.log('native arcoreInstallStatus', require);
        if (!JSBridge) {
            return -1;
        }
        return JSBridge?.arcoreInstallStatus(require);
    }

    /**
     * 
     * @returns 0 support 1 need check 2 unsupport -1 env notready
     */
    static hwarengineSupportStatus() {
        console.log('native hwarengineSupportStatus');
        if (!JSBridge) {
            return -1;
        }
        return JSBridge?.hwarengineSupportStatus();
    }
    
    /**
     * 
     * @param {*} require need install 
     * @returns 0 support 1 need install 2 error -1 env notready
     */
    static hwarengineInstallStatus(require) {
        console.log('native hwarengineInstallStatus', require);
        if (!JSBridge) {
            return -1;
        }
        return JSBridge?.hwarengineInstallStatus(require);
    }

    static selectARSDK(type) {
        if (!JSBridge) {
            return false;
        }
        return JSBridge?.selectARSDK(type);
    }

    /**
     * level
     * @param {*} level 2 == fast, 3 == normal, 4 == extreme
     */
    static selectQuickConfigLevel(level) {
        if (!JSBridge) {
            return false;
        }
        return JSBridge?.selectQuickConfigLevel(level);
    }

    /**
     * return is cloudxr enabled
     */
    static enableCloudXR() {
        if (!JSBridge) {
            return false;
        }
        return JSBridge?.enableCloudXR();
    }
}