

var devices = {};


function createPaintService(htmlCanvas,onSelect) {

    var service = {};                                                                                   // This is the service
    var canvas = htmlCanvas;                                                                            // Reference where the canvas is created
    var ctx = canvas.getContext('2d');                                                                  // Context

    var pps = 10;                                                                                        // Pixels per second
    var my = 50;                                                                                        // Margin top
    var mx = 220;                                                                                       // Margin left
    var tx,px;
    var p1, p2;                                                                                         // Min & max time displayed
    var names = [];                                                                                      // List of names
    var onSel = onSelect;

    canvas.width = canvas.clientWidth; canvas.height = canvas.clientHeight;                             // Set canvas size    
    var cw = canvas.width, ch = canvas.height;                                                         // Shortcut for width & height
    window.addEventListener('resize', function () {                                                     // Make sure that canvas size changes if windows changes
        canvas.width = canvas.clientWidth;
        canvas.height = canvas.clientHeight;
        cw = canvas.width, ch = canvas.height;                                                         // Shortcut for width & height
    }, false);


    ctx.fillStyle = 'black';
    ctx.font = "12px Calibri";
    
    function f(s) {
        if (s > 9) return s; else return "0" + s;
    }

    canvas.onmousemove = function (e) {
        var px = e.offsetX, py = e.offsetY;
        canvas.style.cursor = "default";
        if (px < mx && names.length > 0 && py>my && py < names[names.length - 1].p) canvas.style.cursor = "pointer";

    }


    canvas.onclick = function (e) {
        if (e.offsetX > mx) return;
        var l = e.offsetY + 20, i = 0, n = undefined;
        while (!n && i < names.length)
            if (names[i].p < l && l < (names[i].p + 20)) n= names[i].data; else i++;
        if (n && onSel) onSel(n);
    }

    function paintTime(px, d, s) {
        ctx.fillText(f(s), px-8, my-12);
        if (s == 0) {
            ctx.fillText(f(d.getHours())+":"+f(d.getMinutes()), px-16,my - 24);
        }
    }

    function toDate(s) {
        return new Date(
            s.substring(0, 4), s.substring(5, 7) - 1, s.substring(8, 10),
            s.substring(11, 13),
            s.substring(14, 16),
            s.substring(17, 19));
    }

    function toCircle(centerX, centerY,s) {
        var radius = 5;
        ctx.moveTo(centerX, centerY);
        ctx.beginPath();
        ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
        ctx.fillStyle = 'yellow';
        if (s < 0) s = -s;
        if (s < 55) ctx.fillStyle = 'green';
        if (s > 72) ctx.fillStyle = 'red';
        ctx.fill();
        ctx.stroke();
    }

    function paintMarks(d, ofy) {

        var h = d.h;
        var i = h.length - 1;
        var j = 0;
        while (i >= 0) {
            if (h[i][0] < p1) {
                ctx.beginPath();
                ctx.fillText("[" + i + "] " + h[i][0], cw - 150, ofy);
                ctx.stroke();
                return;
            } else {
                var t1 = Math.floor(toDate(h[i][0]).getTime() / 1000);
                var dif = (tx -t1 );                                                    // Seconds between marker and border right
                var cx = px - dif*pps;
                toCircle(cx, ofy,h[i][1]);
                //ctx.fillText(h[i][0] + "  " + t1,cw-220,ofy);
                //ctx.fillText(tx + " " + t1 + " !! " + cw + " " + px, cw - 160, ofy);
                //return;
            }
            j = j + 1;
            i--;
        }
        ctx.beginPath();
        ctx.fillStyle = 'black';
        ctx.fillText("Samples " + j, cw - 80, ofy);
        ctx.stroke();
    }


    function paintNames() {
        //        var l = Object.getOwnPropertyNames(devices);                                                    // Get all device names
        //for (var i = 0; i < l.length; i++) l[i] = devices[l[i]];                                        // Create a list with all devices

        var l = Object.values(devices);
        l.sort(function (x, y) {
            var r1 = x.last + x.mac;
            var r2 = y.last + y.mac;
            return (r1 > r2 ? -1 : r1 < r2 ? 1 : 0);
        });

        var posy = my + 18;
        var i = 0;
      
        names.length = 0;


        while (i < l.length && posy < ch) {
            var d = l[i];
            ctx.beginPath();
            if (d.mac == "C0:EE:FB:4B:1F:E8") ctx.fillStyle = 'green'; else ctx.fillStyle = 'black';
            ctx.fillText(d.mac, 2, posy);
            ctx.fillText(d.last, 110, posy);
            ctx.moveTo(2, posy + 4); ctx.lineTo(cw, posy + 4);
            ctx.fillStyle = 'black';
            ctx.stroke();

            paintMarks(d, posy);
            names.push({p:posy,data:d});
            posy = posy + 20;
            
            i++;
        }

    }

    service.paint=function() {
        ctx.clearRect(0, 0, cw, ch);                                                                    // Clear rectangle
        ctx.beginPath(); ctx.lineWidth = 1;
        ctx.strokeStyle = '#0000AA';                                                                    // Draw top lines
        ctx.moveTo(0, my); ctx.lineTo(cw, my);
        ctx.moveTo(mx,0); ctx.lineTo(mx, cw);
        ctx.stroke();

        ctx.beginPath();
        ctx.strokeStyle = '#0000FE';                                                                    // Draw top lines

        var d0 = new Date(), d = new Date();                                                                             // This is current time
        var ms = d.getMilliseconds();
        // 1000  -> pps pixeles
        //   ms  -> x
        px = cw- Math.floor(ms * pps / 1000);
        var cx = px;
        while (cx > mx) {
            var s = d.getSeconds();                                                                     // Get seconds
            if (s % 5 == 0) {
                paintTime(cx, d, s);
                ctx.moveTo(cx, my); ctx.lineTo(cx, ch);
            }
            cx = cx - pps;
            d.setSeconds(s - 1);
        }

        p1 = f(d.getFullYear()) + "-" + f(1+d.getMonth()) + "-" + f(d.getDate()) + " " + f(d.getHours()) + ":" + f(d.getMinutes()) + ":" + f(s);
        p2 = f(d0.getFullYear()) + "-" + f(1 + d0.getMonth()) + "-" + f(d0.getDate()) + " " + f(d0.getHours()) + ":" + f(d0.getMinutes()) + ":" + f(d0.getSeconds());
        tx = Math.floor(d0.getTime()/1000);
        ctx.fillText(p1, 2, 12);
        ctx.fillText(p2+" "+tx, 2, 26);

        ctx.stroke();
        paintNames();
    }


    service.start = function () {
        this.timer = setInterval(service.paint,50);
    }

    service.stop = function () {
        if (this.timer) clearInterval(this.timer);
        this.timer = undefined;
    }


    return service;

}
