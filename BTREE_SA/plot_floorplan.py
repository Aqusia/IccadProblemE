#!/usr/bin/env python3
import argparse
import html
from pathlib import Path


def parse_block_outline(block_path):
    with open(block_path, "r", encoding="utf-8") as f:
        tokens = f.read().split()
    if len(tokens) < 3 or tokens[0] != "Outline:":
        raise ValueError("Invalid block file: missing Outline header")
    return int(tokens[1]), int(tokens[2])


def parse_report(report_path):
    with open(report_path, "r", encoding="utf-8") as f:
        lines = [line.strip() for line in f if line.strip()]

    if len(lines) < 5:
        raise ValueError("Invalid report: too few lines")

    cost = float(lines[0])
    wire = float(lines[1])
    area = int(float(lines[2]))
    packed_w, packed_h = map(int, lines[3].split())
    runtime = float(lines[4])

    blocks = []
    for line in lines[5:]:
        parts = line.split()
        if len(parts) != 5:
            continue
        name, x1, y1, x2, y2 = parts
        blocks.append(
            {
                "name": name,
                "x1": int(x1),
                "y1": int(y1),
                "x2": int(x2),
                "y2": int(y2),
            }
        )

    return {
        "cost": cost,
        "wire": wire,
        "area": area,
        "width": packed_w,
        "height": packed_h,
        "runtime": runtime,
        "blocks": blocks,
    }


def color_for_name(name):
    h = 0
    for ch in name:
        h = (h * 131 + ord(ch)) & 0xFFFFFF
    r = 80 + (h & 0x7F)
    g = 80 + ((h >> 8) & 0x7F)
    b = 80 + ((h >> 16) & 0x7F)
    return f"#{r:02x}{g:02x}{b:02x}"


def svg_rect(x, y, w, h, fill, stroke="#222", stroke_width=1, extra=""):
    return (
        f'<rect x="{x:.2f}" y="{y:.2f}" width="{w:.2f}" height="{h:.2f}" '
        f'fill="{fill}" stroke="{stroke}" stroke-width="{stroke_width}" {extra}/>'
    )


def build_svg(report, outline=None, canvas=1200, margin=30):
    world_w = max(report["width"], outline[0] if outline else 0, 1)
    world_h = max(report["height"], outline[1] if outline else 0, 1)
    scale = min((canvas - 2 * margin) / world_w, (canvas - 2 * margin) / world_h)
    view_w = world_w * scale + 2 * margin
    view_h = world_h * scale + 2 * margin + 70

    def sx(x):
        return margin + x * scale

    def sy(y):
        return margin + (world_h - y) * scale

    parts = []
    parts.append(
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{view_w:.0f}" height="{view_h:.0f}" '
        f'viewBox="0 0 {view_w:.0f} {view_h:.0f}">'
    )
    parts.append('<rect width="100%" height="100%" fill="#faf7ef"/>')
    parts.append('<rect x="10" y="10" width="98%" height="98%" fill="none" stroke="#d9d0bf"/>')

    title = (
        f'cost={report["cost"]:.3f}  wire={report["wire"]:.3f}  '
        f'area={report["area"]}  packed={report["width"]}x{report["height"]}  '
        f'runtime={report["runtime"]:.4f}s'
    )
    parts.append(
        f'<text x="{margin}" y="22" font-size="16" font-family="monospace" fill="#222">{html.escape(title)}</text>'
    )

    if outline:
        parts.append(
            svg_rect(
                sx(0),
                sy(outline[1]),
                outline[0] * scale,
                outline[1] * scale,
                "none",
                stroke="#cc3d3d",
                stroke_width=2,
                extra='stroke-dasharray="8 6"',
            )
        )
        outline_text = f"outline={outline[0]}x{outline[1]}"
        parts.append(
            f'<text x="{margin}" y="{view_h - 20:.0f}" font-size="14" font-family="monospace" fill="#aa2e2e">{outline_text}</text>'
        )

    parts.append(
        svg_rect(
            sx(0),
            sy(report["height"]),
            report["width"] * scale,
            report["height"] * scale,
            "none",
            stroke="#2b4c7e",
            stroke_width=2,
        )
    )

    for blk in report["blocks"]:
        x = sx(blk["x1"])
        y = sy(blk["y2"])
        w = (blk["x2"] - blk["x1"]) * scale
        h = (blk["y2"] - blk["y1"]) * scale
        fill = color_for_name(blk["name"])
        parts.append(svg_rect(x, y, w, h, fill, stroke="#222", stroke_width=1))

        if w >= 28 and h >= 14:
            tx = x + w / 2
            ty = y + h / 2 + 4
            parts.append(
                f'<text x="{tx:.2f}" y="{ty:.2f}" text-anchor="middle" font-size="11" '
                f'font-family="monospace" fill="#111">{html.escape(blk["name"])}</text>'
            )

    parts.append("</svg>")
    return "\n".join(parts)


def main():
    parser = argparse.ArgumentParser(description="Render a floorplan report into SVG.")
    parser.add_argument("report", help="Path to output .rpt/.out file")
    parser.add_argument("svg", nargs="?", help="Output SVG path; default: <report>.svg")
    parser.add_argument("--block", help="Optional input .block file to draw fixed outline")
    args = parser.parse_args()

    report_path = Path(args.report)
    svg_path = Path(args.svg) if args.svg else report_path.with_suffix(report_path.suffix + ".svg")

    report = parse_report(report_path)
    outline = parse_block_outline(args.block) if args.block else None
    svg_text = build_svg(report, outline=outline)

    svg_path.parent.mkdir(parents=True, exist_ok=True)
    svg_path.write_text(svg_text, encoding="utf-8")
    print(f"Wrote {svg_path}")


if __name__ == "__main__":
    main()
