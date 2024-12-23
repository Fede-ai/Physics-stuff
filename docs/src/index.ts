const canvas = document.getElementById('graph') as HTMLCanvasElement;
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

const ctx: CanvasRenderingContext2D = (() => {
	let possibleContext = canvas.getContext('2d');

	if (possibleContext)
		return possibleContext;
	else
		throw new Error('Failed to get 2d contex');
})();

interface Point {
	x: number;
	y: number;
}
const lines: Point[][] = [];
const ders: Point[][] = [];
let currentLine: Point[] = [];

const intervals: Point[] = [];
let rightBound: number = Infinity;
let leftBound: number = -Infinity;
let direction: string = '';

let center: Point = {x: window.innerWidth / 2, y: window.innerHeight / 2};
let lastMousePos: Point;
let isMoving = false, isDrawing = false;
let zoom: number = 1;

drawAxes();

//resize and redraw canvas
window.addEventListener('resize', () => {
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;

	resetGraph();
})
//disable context menu (right click is used to move)
window.addEventListener('contextmenu', (event) => {
	event.preventDefault();
})

document.getElementById('back')?.addEventListener('mousedown', (_) => {
	if (!isDrawing && !isMoving && lines.length > 0) {
		lines.pop();
		ders.pop();
		intervals.pop();
		resetGraph();
	}
})

//start drawing or moving
canvas.addEventListener('mousedown', (event) => {
	const x = event.clientX, y = event.clientY;
	//start drawing
	if (event.button == 0 && !isMoving && !isDrawing) {
		const p: Point = {x: (x - center.x) * zoom, y: (y - center.y) * zoom};
		rightBound = Infinity
		leftBound = -Infinity;
		direction = '';

		//dont let functions overlap
		for (let i = 0; i < intervals.length; i++) {
			if (p.x > intervals[i].x && p.x < intervals[i].y)
				return;
			else if (p.x < intervals[i].x)
				rightBound = Math.min(rightBound, intervals[i].x);
			else if (p.x > intervals[i].y)
				leftBound = Math.max(leftBound, intervals[i].y);
		}

		isDrawing = true;
		currentLine.push(p);
	}
	//start moving
	else if (event.button == 2 && !isDrawing) {
		isMoving = true;
		lastMousePos = {x, y};
	}
})
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
})
//zoom
canvas.addEventListener('wheel', (event) => {
	event.preventDefault();
	const startZoom = zoom;
	zoom *= (1 + event.deltaY / 1000);
	zoom = Math.min(Math.max(zoom, 0.15), 3);
	
	const d = zoom / startZoom;
	center.x = (center.x - window.innerWidth / 2) / d + window.innerWidth / 2;
	center.y = (center.y - window.innerHeight / 2) / d + window.innerHeight / 2;

	isDrawing = false;
	finalizeLine(currentLine);
	currentLine = [];
	resetGraph();
})
//stop moving and drawing
canvas.addEventListener('mouseleave', (_) => {
	isMoving = false;
	isDrawing = false;
	finalizeLine(currentLine);
	currentLine = [];
	resetGraph();
})
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
		lastMousePos = {x, y};
		
		resetGraph();
	}
	if (isDrawing && currentTime - lastDraw > 10) {
		const p: Point = {x: (x - center.x) * zoom, y: (y - center.y) * zoom};
		const n = currentLine.length;

		//stop drawing not to everlap other functions
		if (p.x > rightBound || p.x < leftBound ||
			(direction == 'r' && p.x < currentLine[n - 1].x) ||
			(direction == 'l' && p.x > currentLine[n - 1].x)) {
			isDrawing = false;
			finalizeLine(currentLine);
			currentLine = [];
			resetGraph();
			return;
		}

		//draw segment to join the new point
		ctx.beginPath();
		ctx.lineWidth = 1.5;
		ctx.strokeStyle = 'rgb(40 40 240)';
		ctx.moveTo(currentLine[n - 1].x / zoom + center.x, 
			currentLine[n - 1].y / zoom + center.y);
		ctx.lineTo(p.x / zoom + center.x, p.y / zoom + center.y);
		ctx.stroke();

		//prevent multiple points with same x
		if (n > 1 && currentLine[n - 2].x === currentLine[n - 1].x)
			currentLine[n - 1] = p;
		else 
			currentLine.push(p);

		if (direction == '' && p.x > currentLine[n - 1].x)
			direction = 'r';
		else if (direction == '' && p.x < currentLine[n - 1].x)
			direction = 'l';

		lastDraw = currentTime;
	}
})

function finalizeLine(p: Point[]) {
	if (p.length < 10)
		return;

	while (p[p.length - 1].x === p[p.length - 2].x)
		p.pop();

	//smoothen the line
	const smooth: Point[] = [];
	for (let i = 0; i < p.length; i++) {
    let sumX = 0;
    let sumY = 0;
    let count = 0;
		
    for (let j = -3; j <= 3; j++) {
      const idx = i + j;
      if (idx >= 0 && idx < p.length) {
        sumX += p[idx].x;
        sumY += p[idx].y;
        count++;
      }
    }

    smooth.push({
      x: sumX / count,
      y: sumY / count
    });
  }
	lines.push(smooth);

	let interval: Point = { x: 0, y: 0 };
	interval.x = Math.min(smooth[0].x, smooth[smooth.length - 1].x);
	interval.y = Math.max(smooth[0].x, smooth[smooth.length - 1].x);
	intervals.push(interval);

	//compute the derivative line
	const der: Point[] = [];
	const derValues = getDerivatives(smooth);
	for (let i = 0; i < p.length; i++)
		der.push({ x: smooth[i].x, y: derValues[i] });
	ders.push(der);
}
function resetGraph(): void {
	ctx.clearRect(0, 0, canvas.width, canvas.height);
	drawAxes();

	if (lines.length !== ders.length)
		throw new Error("Lines and ders don't match");

	for (let i = 0; i < lines.length; i++)
		drawLine(lines[i], ders[i]);
}
function drawAxes(): void {
	ctx.beginPath();
	ctx.strokeStyle = 'rgb(100 100 100)';
	ctx.lineWidth = 0.5;

	//draw vertical axes
	const xDiff = 50 / zoom;
	let mid = center.x - Math.floor(center.x / xDiff) * xDiff;
	for (let x = 0; x < window.innerWidth; x += xDiff) {
		if (mid + x == center.x)
			continue;

		ctx.moveTo(mid + x, 0);
		ctx.lineTo(mid + x, window.innerHeight);
	}
	//draw horizontal axes
	const yDiff = 50 / zoom;
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
function drawLine(l: Point[], d: Point[]): void {
	ctx.lineWidth = 1.5;
	const yDiff = 50 / zoom;

	ctx.beginPath();
	ctx.strokeStyle = 'rgb(40 240 40)';
	ctx.moveTo(l[0].x / zoom + center.x, l[0].y / zoom + center.y);
	for (let i = 1; i < l.length; i++)
		ctx.lineTo(l[i].x / zoom + center.x, l[i].y / zoom + center.y);
	ctx.stroke();

	ctx.beginPath();
	ctx.strokeStyle = 'rgb(240 40 40)';
	ctx.moveTo(d[0].x / zoom + center.x, d[0].y * yDiff + center.y);
	for (let i = 1; i < d.length; i++)
		ctx.lineTo(d[i].x / zoom + center.x, d[i].y * yDiff + center.y);
	ctx.stroke();
}

//reference: https://en.wikipedia.org/wiki/Akima_spline
function getDerivatives(p: Point[]): number[] {
	const m: number[] = [];
	for (let i = 0; i < p.length - 1; i++)
		m.push((p[i + 1].y - p[i].y) / (p[i + 1].x - p[i].x));

	const s: number[] = [];
	//special handling for s(1) and s(2)
	s.push(m[0]);
	s.push((m[0] + m[1]) / 2);
	for (let i = 2; i < p.length - 2; i++) {
		let a = Math.abs(m[i + 1] - m[i]);
		let b = Math.abs(m[i - 1] - m[i - 2]);
		let den = a + b;

		if (den == 0)
			s.push((m[i - 1] + m[i]) / 2);
		else
			s.push((a * m[i - 1] + b * m[i]) / den)
	}
	//special handling for s(n-1) and s(n)
	s.push((m[p.length - 3] + m[p.length - 2]) / 2);
	s.push(m[p.length - 2]);

	return s;
}