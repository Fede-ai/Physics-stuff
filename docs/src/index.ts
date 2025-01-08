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
let isMoving = false, isDrawing = false, isErasing = false;
let traceMode = 'd'; //drawing is selected at first
let zoom: number = 1;
let touchesDistance: number = 0;
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

const buttons = document.getElementById('buttons') as HTMLDivElement;
const cr = document.getElementById('cr') as HTMLDivElement;
const ver = document.getElementById('ver') as HTMLDivElement;
ver.textContent = 'v 1.2.4'
buttons.addEventListener('wheel', (event) => {
	event.preventDefault();
})
cr.addEventListener('wheel', (event) => {
	event.preventDefault();
})
ver.addEventListener('wheel', (event) => {
	event.preventDefault();
})
buttons.addEventListener('touchmove', (event) => {
	event.preventDefault();
})
cr.addEventListener('touchmove', (event) => {
	event.preventDefault();
})
ver.addEventListener('touchmove', (event) => {
	event.preventDefault();
})

const back = document.getElementById('back') as HTMLDivElement;
const draw = document.getElementById('draw') as HTMLDivElement;
draw.style.borderColor = "rgb(160, 40, 40)"
const erase = document.getElementById('erase') as HTMLDivElement;
const move = document.getElementById('move') as HTMLDivElement;
back.addEventListener('mousedown', (event) => {
	event.preventDefault();
	
	if (!isDrawing && !isMoving && lines.length > 0) {
		lines.pop();
		ders.pop();
		intervals.pop();
		resetGraph();
	}
})
draw.addEventListener('mousedown', (event) => {
	event.preventDefault();
	
	traceMode = 'd'
	draw.style.borderColor = "rgb(160, 40, 40)"
	erase.style.borderColor = "rgb(50, 50, 50)"
	move.style.borderColor = "rgb(50, 50, 50)"
})
erase.addEventListener('mousedown', (event) => {
	event.preventDefault();
	
	traceMode = 'e'
	draw.style.borderColor = "rgb(50, 50, 50)"
	erase.style.borderColor = "rgb(160, 40, 40)"
	move.style.borderColor = "rgb(50, 50, 50)"
})
move.addEventListener('mousedown', (event) => {
	event.preventDefault();
	
	traceMode = 'm'
	erase.style.borderColor = "rgb(50, 50, 50)"
	draw.style.borderColor = "rgb(50, 50, 50)"
	move.style.borderColor = "rgb(160, 40, 40)"
})

//start drawing/erasing or moving
canvas.addEventListener('mousedown', (event) => {
	startAction(event.clientX, event.clientY, event.button);
})
canvas.addEventListener('touchstart', (event) => {
	if (event.touches.length == 1) {
		let touch = event.touches[0];
		startAction(touch.clientX, touch.clientY, 0);
	}
	else if (event.touches.length == 2) {
		let t1 = event.touches[0], t2 = event.touches[1];
		const dx = t2.clientX - t1.clientX;
		const dy = t2.clientY - t1.clientY;
		touchesDistance =  Math.sqrt(dx * dx + dy * dy);
	}
})
function startAction(x: number, y: number, b: number) {
	//start drawing or erasing
	if (b == 0 && !isMoving && !isDrawing && !isErasing) {
		//start drawing if initial position is valid
		if (traceMode == 'd') {
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
		//start erasing
		else if (traceMode == 'e') {
			ctx.beginPath();
			ctx.fillStyle = 'rgba(255, 255, 255, 0.2)'
			ctx.arc(x, y, 25, 0, 2 * Math.PI);
			ctx.fill();

			eraseCircle(x, y);
			isErasing = true;
		}
		//start moving
		else if (traceMode == 'm') {
			isMoving = true;
			lastMousePos = {x, y};
		}
	}
	//start moving
	else if (b == 2 && !isDrawing && !isErasing) {
		isMoving = true;
		lastMousePos = {x, y};
	}
}

//write, draw, move or zoom
canvas.addEventListener('mousemove', (event) => {
	performAction(event.clientX, event.clientY)
})
canvas.addEventListener('touchmove', (event) => {
	event.preventDefault();
	if (event.touches.length == 1) {
		let touch = event.touches[0];
		performAction(touch.clientX, touch.clientY)
	}
	else if (event.touches.length == 2) {
		let t1 = event.touches[0], t2 = event.touches[1];
		const dx = t2.clientX - t1.clientX;
		const dy = t2.clientY - t1.clientY;
		const newDistance =  Math.sqrt(dx * dx + dy * dy);

		performZoom((touchesDistance - newDistance) * 3);
		touchesDistance = newDistance;
	}
})
canvas.addEventListener('wheel', (event) => {
	event.preventDefault();
	performZoom(event.deltaY);
})
function performZoom(delta: number) {
	const startZoom = zoom;
	zoom *= (1 + delta / 1000);
	zoom = Math.min(Math.max(zoom, 0.1), 3);
	
	const d = zoom / startZoom;
	center.x = (center.x - window.innerWidth / 2) / d + window.innerWidth / 2;
	center.y = (center.y - window.innerHeight / 2) / d + window.innerHeight / 2;

	if (isDrawing) {
		isDrawing = false;
		finalizeLine(currentLine);
		currentLine = [];
	}
	isErasing = false;
	
	resetGraph();
}
let now = performance.now();
let lastMove = now, lastDraw = now, lastErase = now;
const moveInt = 25, drawInt = 8, eraseInt = 40;
function performAction(x: number, y: number) {
	const currentTime = performance.now();

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
	if (isErasing && currentTime - lastErase > eraseInt) {
		resetGraph();

		ctx.beginPath();
		ctx.fillStyle = 'rgba(255, 255, 255, 0.2)'
		ctx.arc(x, y, 25, 0, 2 * Math.PI);
		ctx.fill();

		eraseCircle(x, y);
		lastErase = currentTime;
	}
}

//stop drawing/erasing or moving
canvas.addEventListener('mouseleave', (_) => {
	endAction();
})
canvas.addEventListener('touchend', (_) => {
	endAction();
})
canvas.addEventListener('touchcancel', (_) => {
	endAction();
})
function endAction() {
	if (isDrawing) {
		isDrawing = false;
		finalizeLine(currentLine);
		currentLine = [];
		resetGraph();
	}
	if (isErasing) {
		isErasing = false;
		resetGraph();
	}
	isMoving = false;
}
//this case is handled apart
canvas.addEventListener('mouseup', (event) => {
	if (event.button == 0) {
		if (isDrawing) {
			isDrawing = false;
			finalizeLine(currentLine);
			currentLine = [];
			resetGraph();
		}
		if (isErasing) {
			isErasing = false;
			resetGraph();
		}
		//stop moving if right button is not pressed
		if (isMoving && (event.buttons & 2) != 2) {
			isMoving = false;
		}
	}
	//stop moving if left button is not pressed
	else if (event.button == 2 && ((event.buttons & 1) != 1 || traceMode != 'm')) {
		isMoving = false;
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
	const smooth: Point[] = smoothen(p, 4);
	let spline = akimaSpline(smooth);
	
	let line: Point[] = [];
	//use the spline to evenly space the points
	for (let x = smooth[0].x; x < smooth[smooth.length - 1].x; x += zoom)
		line.push({ x, y: spline(x) });
	//add additional smoothening
	line = smoothen(line, 4);

	//remove line if too short
	if (line.length < 20)
		return;
	
	//calculate slopes between every point
	const m: number[] = [];
	for (let i = 0; i < line.length - 1; i++)
		m.push((line[i + 1].y - line[i].y) / (line[i + 1].x - line[i].x));
		
	const s: number[] = [];
	//special handling for s(1) and s(2)
	s.push(m[0]);
	s.push((m[0] + m[1]) / 2);
	//calculate derivative at every point
	for (let i = 2; i < line.length - 2; i++) {
		let a = Math.abs(m[i + 1] - m[i]);
		let b = Math.abs(m[i - 1] - m[i - 2]);
		let den = a + b;
		
		if (den == 0)
			s.push((m[i - 1] + m[i]) / 2);
		else
			s.push((a * m[i - 1] + b * m[i]) / den)
	}
	//special handling for s(n-1) and s(n)
	s.push((m[line.length - 3] + m[line.length - 2]) / 2);
	s.push(m[line.length - 2]);

	//create derivative line and smoothen it
	let der: Point[] = [];
	for (let i = 0; i < line.length; i++)
		der.push({ x: line[i].x, y: s[i] });
	der = smoothen(der, 10);
	
	//join the line with one close to its left
	for (let i = 0; i < lines.length; i++) {
		let dx = line[0].x - lines[i][lines[i].length - 1].x;
		let dy = line[0].y - lines[i][lines[i].length - 1].y;

		if (Math.sqrt(dx**2 + dy**2) < 25 * zoom) {
			for (let j = 0; j < line.length; j++) {
				line[j].x -= dx;
				line[j].y -= dy;
				der[j].x -= dx;
			}
			break;
		}
	}
	
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
function eraseCircle(x: number, y: number): void {
	const cx = (x - center.x) * zoom;
	const cy = (y - center.y) * zoom;

	//loop through all the lines
	for (let i = 0; i < lines.length; i++) {
		//skip if line is far from the mouse
		if (intervals[i].start > cx + 25 * zoom || 
			intervals[i].end < cx - 25 * zoom)
			continue;

		let deletedLine = false;
		//erase points from the start of the line
		while (!deletedLine) {
			let dx = (lines[i][0].x - cx) ** 2;
			let dy = (lines[i][0].y - cy) ** 2;
			if (Math.sqrt(dx + dy) < 25 * zoom) {
				//erase the whole line (= cus it will decrease by 1)
				if (lines[i].length <= 20) {
					lines.splice(i, 1)
					ders.splice(i, 1);
					intervals.splice(i, 1);
					deletedLine = true;
				}
				//erase one point
				else {
					lines[i].shift();
					ders[i].shift();
					intervals[i].start = lines[i][0].x;
				}
			}
			else
				break;
		}
		//erase points from the end of the line
		while (!deletedLine) {
			let dx = (lines[i][lines[i].length - 1].x - cx) ** 2;
			let dy = (lines[i][lines[i].length - 1].y - cy) ** 2;
			if (Math.sqrt(dx + dy) < 25 * zoom) {
				//erase the whole line (= cus it will decrease by 1)
				if (lines[i].length <= 20) {
					lines.splice(i, 1)
					ders.splice(i, 1);
					intervals.splice(i, 1);
					deletedLine = true;
				}
				//erase one point
				else {
					lines[i].pop();
					ders[i].pop();
					intervals[i].end = lines[i][lines[i].length - 1].x;
				}
			}
			else
				break;
		}

		if (deletedLine)
			continue;

		//erase points from the middle of the line
		for (let j = 0; j < lines[i].length; j++) {
			let dx = (lines[i][j].x - cx) ** 2;
			let dy = (lines[i][j].y - cy) ** 2;
			if (Math.sqrt(dx + dy) < 25 * zoom) {
				splitLine(i, j);
				//loop again throught the last line
				i--;
				break;
			}
		}
	}
}
function splitLine(l: number, p: number): void {
	intervals.splice(l, 1);
	let l2: Point[] = lines.splice(l, 1)[0];
	let l1: Point[] = l2.splice(0, p);
	l2.splice(0, 1);
	
	let d2: Point[] = ders.splice(l, 1)[0];
	let d1: Point[] = d2.splice(0, p);
	d2.splice(0, 1);

	if (l1.length >= 20) {
		lines.push(l1);
		ders.push(d1);
		intervals.push({ start: l1[0].x, end: l1[l1.length - 1].x })
	}
	if (l2.length >= 20) {
		lines.push(l2);
		ders.push(d2);
		intervals.push({ start: l2[0].x, end: l2[l2.length - 1].x })
	}
}

//reference: https://en.wikipedia.org/wiki/Akima_spline
function akimaSpline(p: Point[]): (x: number) => number {
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
			return y;
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