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
let points = [[]];
let center = { x: window.innerWidth / 2, y: window.innerHeight / 2 };
let lastMousePos;
let isMoving = false, isDrawing = false;
drawAxes();
//resize and redraw canvas
window.addEventListener('resize', () => {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    drawAxes();
    for (let i = 0; i < points.length - 1; i++)
        drawLine(points[i]);
});
//disable context menu (right click is used to move)
window.addEventListener('contextmenu', (event) => {
    event.preventDefault();
});
//start drawing or moving
canvas.addEventListener('mousedown', (event) => {
    const x = event.clientX, y = event.clientY;
    if (event.button == 0) {
        isDrawing = true;
        const p = { x: x - center.x, y: y - center.y };
        if (points[points.length - 1].length > 0)
            drawLine([points[points.length - 1][points[points.length - 1].length - 1], p]);
        points[points.length - 1].push(p);
    }
    else if (event.button == 2 && !isDrawing) {
        isMoving = true;
        lastMousePos = { x, y };
    }
});
//stop moving
canvas.addEventListener('mouseup', (event) => {
    if (event.button == 0) {
        isDrawing = false;
        if (points[points.length - 1].length > 0)
            points.push([]);
    }
    else if (event.button == 2) {
        isMoving = false;
    }
});
//stop moving
canvas.addEventListener('mouseleave', (_) => {
    isDrawing = false;
    if (points[points.length - 1].length > 0)
        points.push([]);
    isMoving = false;
});
//move canvas is right click is down
let lastMove = performance.now();
let lastDraw = performance.now();
canvas.addEventListener('mousemove', (event) => {
    const currentTime = performance.now();
    const x = event.clientX, y = event.clientY;
    if (isMoving && currentTime - lastMove > 50) {
        center.x += x - lastMousePos.x;
        center.y += y - lastMousePos.y;
        lastMove = currentTime;
        lastMousePos = { x, y };
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        drawAxes();
        for (let i = 0; i < points.length - 1; i++)
            drawLine(points[i]);
    }
    if (isDrawing && currentTime - lastDraw > 30) {
        const p = { x: x - center.x, y: y - center.y };
        if (points[points.length - 1].length > 0)
            drawLine([points[points.length - 1][points[points.length - 1].length - 1], p]);
        points[points.length - 1].push(p);
        lastDraw = currentTime;
    }
});
function drawAxes() {
    ctx.beginPath();
    ctx.strokeStyle = 'rgb(100 100 100)';
    ctx.lineWidth = 1;
    //draw vertical axes
    const xDiff = 100;
    let mid = center.x - Math.floor(center.x / xDiff) * xDiff;
    for (let x = 0; x < window.innerWidth; x += xDiff) {
        if (mid + x == center.x)
            continue;
        ctx.moveTo(mid + x, 0);
        ctx.lineTo(mid + x, window.innerHeight);
    }
    //draw horizontal axes
    const yDiff = 100;
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
function drawLine(ps) {
    ctx.beginPath();
    ctx.strokeStyle = 'rgb(40 240 40)';
    ctx.moveTo(ps[0].x + center.x, ps[0].y + center.y);
    for (let i = 1; i < ps.length; i++) {
        ctx.lineTo(ps[i].x + center.x, ps[i].y + center.y);
    }
    ctx.stroke();
}
