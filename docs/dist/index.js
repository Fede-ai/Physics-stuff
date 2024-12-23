"use strict";
const canvas = document.getElementById('graph');
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;
const ctx = (() => {
    let possibleContext = canvas.getContext('2d');
    if (possibleContext)
        return possibleContext;
    else
        throw new Error("Failed to get 2d contex");
})();
const lines = [];
const ders = [];
let currentLine = [];
let center = { x: window.innerWidth / 2, y: window.innerHeight / 2 };
let lastMousePos;
let isMoving = false, isDrawing = false;
let zoom = 1;
drawAxes();
//resize and redraw canvas
window.addEventListener('resize', () => {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    resetGraph();
});
//disable context menu (right click is used to move)
window.addEventListener('contextmenu', (event) => {
    event.preventDefault();
});
//start drawing or moving
canvas.addEventListener('mousedown', (event) => {
    const x = event.clientX, y = event.clientY;
    //start drawing
    if (event.button == 0 && !isMoving) {
        const p = { x: (x - center.x) * zoom, y: (y - center.y) * zoom };
        isDrawing = true;
        currentLine.push(p);
    }
    //start moving
    else if (event.button == 2 && !isDrawing) {
        isMoving = true;
        lastMousePos = { x, y };
    }
});
//stop moving
canvas.addEventListener('mouseup', (event) => {
    if (event.button == 0) {
        isDrawing = false;
        finalizeLine(currentLine);
        currentLine = [];
        resetGraph();
    }
    else if (event.button == 2) {
        isMoving = false;
    }
});
//zoom
canvas.addEventListener('wheel', (event) => {
    event.preventDefault();
    const startZoom = zoom;
    zoom *= (1 + event.deltaY / 1000);
    zoom = Math.min(Math.max(zoom, 0.2), 5);
    const d = zoom / startZoom;
    center.x = (center.x - window.innerWidth / 2) / d + window.innerWidth / 2;
    center.y = (center.y - window.innerHeight / 2) / d + window.innerHeight / 2;
    isDrawing = false;
    finalizeLine(currentLine);
    currentLine = [];
    resetGraph();
});
//stop moving and drawing
canvas.addEventListener('mouseleave', (_) => {
    isMoving = false;
    isDrawing = false;
    finalizeLine(currentLine);
    currentLine = [];
    resetGraph();
});
//move canvas if right click is down
let lastMove = performance.now();
let lastDraw = performance.now();
canvas.addEventListener('mousemove', (event) => {
    const currentTime = performance.now();
    const x = event.clientX, y = event.clientY;
    if (isMoving && currentTime - lastMove > 40) {
        center.x += x - lastMousePos.x;
        center.y += y - lastMousePos.y;
        lastMove = currentTime;
        lastMousePos = { x, y };
        resetGraph();
    }
    if (isDrawing && currentTime - lastDraw > 15) {
        const p = { x: (x - center.x) * zoom, y: (y - center.y) * zoom };
        //draw segment to join the new point
        if (currentLine.length > 0) {
            ctx.beginPath();
            ctx.lineWidth = 1.5;
            ctx.strokeStyle = 'rgb(40 40 240)';
            ctx.moveTo(currentLine[currentLine.length - 1].x / zoom + center.x, currentLine[currentLine.length - 1].y / zoom + center.y);
            ctx.lineTo(p.x / zoom + center.x, p.y / zoom + center.y);
            ctx.stroke();
        }
        currentLine.push(p);
        lastDraw = currentTime;
    }
});
function finalizeLine(ps) {
    if (ps.length < 10)
        return;
    const smooth = [];
    //smoothen the line
    for (let i = 0; i < ps.length; i++) {
        let sumX = 0;
        let sumY = 0;
        let count = 0;
        for (let j = -3; j <= 3; j++) {
            const idx = i + j;
            if (idx >= 0 && idx < ps.length) {
                sumX += ps[idx].x;
                sumY += ps[idx].y;
                count++;
            }
        }
        smooth.push({
            x: sumX / count,
            y: sumY / count
        });
    }
    const der = [];
    for (let i = 0; i < ps.length - 1; i++) {
        let x = (smooth[i].x + smooth[i + 1].x) / 2;
        let y = (smooth[i + 1].y - smooth[i].y) / (smooth[i + 1].x - smooth[i].x);
        der.push({ x, y });
    }
    lines.push(smooth);
    ders.push(der);
}
function resetGraph() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawAxes();
    if (lines.length !== ders.length)
        throw new Error("Lines and ders don't match");
    for (let i = 0; i < lines.length; i++)
        drawLine(lines[i], ders[i]);
}
function drawAxes() {
    ctx.beginPath();
    ctx.strokeStyle = 'rgb(100 100 100)';
    ctx.lineWidth = 0.5;
    //draw vertical axes
    const xDiff = 100 / zoom;
    let mid = center.x - Math.floor(center.x / xDiff) * xDiff;
    for (let x = 0; x < window.innerWidth; x += xDiff) {
        if (mid + x == center.x)
            continue;
        ctx.moveTo(mid + x, 0);
        ctx.lineTo(mid + x, window.innerHeight);
    }
    //draw horizontal axes
    const yDiff = 100 / zoom;
    mid = center.y - Math.floor(center.y / yDiff) * yDiff;
    for (let y = 0; y < window.innerHeight; y += yDiff) {
        if (mid + y == center.y)
            continue;
        ctx.moveTo(0, mid + y);
        ctx.lineTo(window.innerWidth, mid + y);
    }
    ctx.stroke();
    ctx.beginPath();
    ctx.strokeStyle = 'white';
    ctx.lineWidth = 1.5;
    ctx.moveTo(0, center.y);
    ctx.lineTo(window.innerWidth, center.y);
    ctx.moveTo(center.x, 0);
    ctx.lineTo(center.x, window.innerHeight);
    ctx.stroke();
}
function drawLine(l, d) {
    ctx.lineWidth = 1.5;
    const yDiff = 100 / zoom;
    ctx.beginPath();
    ctx.strokeStyle = 'rgb(40 240 40)';
    ctx.moveTo(l[0].x / zoom + center.x, l[0].y / zoom + center.y);
    for (let i = 1; i < l.length; i++)
        ctx.lineTo(l[i].x / zoom + center.x, l[i].y / zoom + center.y);
    ctx.stroke();
    ctx.beginPath();
    ctx.strokeStyle = 'rgb(240 40 40)';
    ctx.moveTo(d[0].x / zoom + center.x, d[0].y * yDiff + center.y);
    for (let i = 1; i < d.length - 1; i++)
        ctx.lineTo(d[i].x / zoom + center.x, d[i].y * yDiff + center.y);
    ctx.stroke();
}
