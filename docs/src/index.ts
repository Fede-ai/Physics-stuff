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

type Point = { x: number, y: number }
const lines: Point[][] = [];
const ders: Point[][] = [];
let currentLine: Point[] = [];

type Interval = { start: number, end: number }
const intervals: Interval[] = [];
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

		//calculate the current available interval
		for (let i = 0; i < intervals.length; i++) {
			if (p.x > intervals[i].start && p.x < intervals[i].end)
				return;
			else if (p.x < intervals[i].start)
				rightBound = Math.min(rightBound, intervals[i].start);
			else if (p.x > intervals[i].end)
				leftBound = Math.max(leftBound, intervals[i].end);
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
	zoom = Math.min(Math.max(zoom, 0.1), 3);
	
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
let moveInt = 25, drawInt = 10;
canvas.addEventListener('mousemove', (event) => {
	const currentTime = performance.now();
	const x = event.clientX, y = event.clientY;

	if (isMoving && currentTime - lastMove > moveInt) {
		center.x += x - lastMousePos.x;
		center.y += y - lastMousePos.y;
		
		lastMove = currentTime;
		lastMousePos = {x, y};
		
		resetGraph();
	}
	if (isDrawing && currentTime - lastDraw > drawInt) {
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
		ctx.lineWidth = 2;
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
	//avoid crash when removing tail
	if (p.length < 5)
		return;

	//remove ending vertical tail
	while (p[p.length - 1].x === p[p.length - 2].x)
		p.pop();

	//avoid crash after smoothening
	if (p.length < 5)
		return;

	if (direction == 'l')
		p = p.reverse();

	//smoothen the line
	const smooth: Point[] = smoothen(p, 3);
	//since the smoothening messes up the derivative at the extremities
	let derInt: Interval = { start: smooth[3].x, end: smooth[smooth.length - 3].x };
	let spline = akimaSpline(smooth);
	
	let line: Point[] = [];
	let der: Point[] = [];
	//use the spline to evenly space the points
	for (let x = smooth[0].x; x < smooth[smooth.length - 1].x; x++) {
		let a = spline(x);
		line.push({ x, y: a.y });
		if (x > derInt.start && x < derInt.end)
			der.push({ x, y: a.d });
	}

	//remove line if too short
	if (line.length <= 20)
		return;

	//correct for missing derivate at the extremities
	let ms = (der[0].y - der[1].y) / (der[0].x - der[1].x);
	for (let x = derInt.start; x > smooth[0].x; x--)
		der.unshift({ x, y: der[0].y - ms});
	let n = der.length - 1;
	let me = (der[n - 1].y - der[n].y) / (der[n - 1].x - der[n].x);
	for (let x = derInt.end; x < smooth[smooth.length - 1].x; x++)
		der.push({ x, y: der[der.length - 1].y + me});

	//add additional smoothening
	line = smoothen(line, 3);
	der = smoothen(der, 9);

	//calculate the domain
	let xs = line[0].x;
	let xe = line[line.length - 1].x;
	intervals.push({ start: xs, end: xe });	

	lines.push(line);
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
	ctx.lineWidth = 2;
	ctx.moveTo(0, center.y);
	ctx.lineTo(window.innerWidth, center.y);
	ctx.moveTo(center.x, 0);
	ctx.lineTo(center.x, window.innerHeight);
	ctx.stroke();
}
function drawLine(l: Point[], d: Point[]): void {
	ctx.lineWidth = 2;
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
type SplinePoint = { y: number, d: number}
function akimaSpline(p: Point[]): (x: number) => SplinePoint {
	//calculate slopes between every point
	const m: number[] = [];
	for (let i = 0; i < p.length - 1; i++)
		m.push((p[i + 1].y - p[i].y) / (p[i + 1].x - p[i].x));

	const s: number[] = [];
	//special handling for s(1) and s(2)
	s.push(m[0]);
	s.push((m[0] + m[1]) / 2);
	//calculate derivative at every point
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

	return function(x: number) {
		for (let i = 0; i < p.length - 1; i++) {
			if (x > p[i + 1].x)
				continue;
			//return { y: p[i].y, d: s[i] };

			let a = p[i].y;
			let b = s[i];
			let c = (3 * m[i] - 2 * s[i] - s[i + 1]) / (p[i + 1].x - p[i].x);
			let d = (s[i] + s[i + 1] - 2 * m[i]) / (p[i + 1].x - p[i].x) ** 2;

			let dx = x - p[i].x;
			let y = a + b * dx + c * dx ** 2 + d * dx ** 3;
			let der = b + 2 * c * dx + 3 * d * dx ** 2;
			return { y, d: der };
		}
		throw Error("Bug with Akima spline");
	}
}
function smoothen(p: Point[], n: number): Point[] {
	//initial subtle smoothening
	let s: Point[] = [];
	s.push(p[0]);
	for (let i = 1; i < p.length - 1; i++) {
		let x = (p[i - 1].x + p[i].x + p[i + 1].x) / 3;
		let y = (p[i - 1].y + p[i].y + p[i + 1].y) / 3;
		s.push({ x, y });
	}
	s.push(p[p.length - 1]);
	
	//extend both ends to avoid contraction
	let x1 = s[0].x - s[1].x;
	let y1 = s[0].y - s[1].y;
	let x2 = s[s.length - 1].x - s[s.length - 2].x;
	let y2 = s[s.length - 1].y - s[s.length - 2].y;
	for (let i = 0; i < n; i++) {
		s.unshift({ x: s[0].x + x1, y: s[0].y + y1 });
		s.push({ x: s[s.length - 1].x + x2, y: s[s.length - 1].y + y2 });
	}

	//perform actual smoothening
	const smooth: Point[] = [];
	for (let i = n; i < s.length - n; i++) {
    let xSum = 0, ySum = 0;
    for (let j = -n; j <= n; j++) {
      const idx = i + j;
      xSum += s[idx].x;
      ySum += s[idx].y;
    }
    smooth.push({ x: xSum / (2 * n + 1), y: ySum / (2 * n + 1) });
  }
	return smooth;
}