if (typeof console === "undefined" || typeof console.log === "undefined") {
    console = {};
    console.log = function() {};
}

if (typeof console === "undefined" || typeof console.dir === "undefined") {
    console = {};
    console.dir = function() {};
}

$(document).bind("mobileinit", function() {
    $.ajaxSetup({ cache: false });
    $.ajaxSetup({ isLocal: true });

    $.mobile.defaultPageTransition = 'slide';
});
