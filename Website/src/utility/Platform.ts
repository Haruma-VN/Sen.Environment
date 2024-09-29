const detectOS = (): "android" | "ios" | "windows" | "macintosh" | "linux" | "unknown" => {
    const userAgent = window.navigator.userAgent;
    if (/android/i.test(userAgent)) {
        return "android";
    }
    if (/iPad|iPhone|iPod/.test(userAgent)) {
        return "ios";
    }
    if (/Win/.test(userAgent)) {
        return "windows";
    }
    if (/Mac/.test(userAgent)) {
        return "macintosh";
    }
    if (/Linux/.test(userAgent)) {
        return "linux";
    }
    return "unknown";
};

export default detectOS;
