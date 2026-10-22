#!/usr/bin/env python3
"""Prepare source-preserving printable assets from existing YaleCourses video frames."""
from pathlib import Path
from PIL import Image, ImageOps, ImageDraw, ImageFont
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import landscape, A4
from reportlab.lib.utils import ImageReader
import json, shutil, zipfile, textwrap

BASE=Path("parenting/everyday-parenting")
ORIG=BASE/"original"
PRINT=BASE/"print"
ORIG.mkdir(parents=True,exist_ok=True)
PRINT.mkdir(parents=True,exist_ok=True)

RECORDS=[
("01_Praise","Praise", "complete"),
("02_Antecedents","Antecedents","complete"),
("03_Shaping","Shaping","incomplete"),
("04_Simulation","Simulation","complete"),
("05_Modeling","Modeling","complete"),
("06_Positive_Opposite","Positive Opposite","incomplete"),
("07_Point_Programs","Point Programs","incomplete"),
("08_Attending","Attending","incomplete"),
("09_Planned_Ignoring","Planned Ignoring","complete"),
("10_Punishment","Punishment","incomplete"),
("11_Time_Out","Time Out","complete")]
# Images are video frames at 640x360. Never claim their detail can be made genuinely hi-res.
for name,title,status in RECORDS:
    source=BASE/f"{name}.png"
    if not source.exists():raise FileNotFoundError(source)
    img=Image.open(source)
    img.verify()
    shutil.copy2(source,ORIG/source.name)
    with Image.open(source) as opened:
        img=opened.convert("RGB")
        # White space is part of lesson illustrations: preserve the original content.
        # Print at native resolution, avoiding artificial 'HD' upscaling.
        margin=48
        header=115
        canvas_img=Image.new("RGB",(1440,980),"white")
        d=ImageDraw.Draw(canvas_img)
        # Fit preserving aspect, no content crop
        target=(1344,756)
        resized=ImageOps.contain(img,target,Image.Resampling.LANCZOS)
        x=(1440-resized.width)//2
        y=130+(756-resized.height)//2
        canvas_img.paste(resized,(x,y))
        try:
            font=ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",40)
            font_small=ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",24)
        except OSError:
            font=ImageFont.load_default();font_small=font
        d.text((margin,30),title,fill="#1e293b",font=font)
        if status!="complete":
            d.text((margin,910),"VIDEO STILL - complete illustrated steps not yet verified",fill="#ad3e1d",font=font_small)
        else:
            d.text((margin,910),"Original YaleCourses illustration | Source video frame",fill="#64748b",font=font_small)
        canvas_img.save(PRINT/(name+".png"),optimize=True)

pdf=BASE/"Everyday_Parenting_A4.pdf"
c=canvas.Canvas(str(pdf),pagesize=landscape(A4))
pw,ph=landscape(A4)
for name,title,status in RECORDS:
    p=PRINT/(name+".png")
    iw,ih=Image.open(p).size
    scale=min((pw-25)/iw,(ph-25)/ih)
    w,h=iw*scale,ih*scale
    c.drawImage(str(p),(pw-w)/2,(ph-h)/2,width=w,height=h)
    c.showPage()
c.setTitle("Everyday Parenting - 11 frame printing edition")
c.save()

readme=BASE/"PRINT_README.md"
readme.write_text("""# Everyday Parenting: printable collection

This package includes **all 11 original video frame captures**, but this is **not** a confirmed set of 11 complete course illustrations.

- `original/`: exact image copies, no enhancement.
- `print/`: 11 consistent landscape cards; preserve source image pixels without cropping.
- `Everyday_Parenting_A4.pdf`: 11 landscape A4 pages. Six complete illustrated cards, five **explicitly marked incomplete**.
- `Everyday_Parenting_Print.zip`: all of the above.

## Frame review

| Topic | Complete original graphic at captured time? |
|---|---|
"""+"\n".join(f"| {title} | {'Yes' if status=='complete' else 'No - partial step or instructor scene'} |" for _,title,status in RECORDS)+"""

All images are sourced from publicly available YaleCourses videos. Original PNGs are 640x360, so the print version **does not increase real detail**. This is an archival / interim printable set. Five slides require new complete-graphic captures before a truly final 11/11 edition is possible.
""",encoding="utf8")
archive=BASE/"Everyday_Parenting_Print.zip"
with zipfile.ZipFile(archive,"w",zipfile.ZIP_DEFLATED,compresslevel=7) as z:
    for group in (ORIG,PRINT):
        for p in sorted(group.glob("*.png")):
            z.write(p,p.relative_to(BASE))
    z.write(pdf,pdf.name)
    z.write(readme,readme.name)
assert len(list(ORIG.glob("*.png")))==11
assert len(list(PRINT.glob("*.png")))==11
print(json.dumps({"original_count":11,"print_count":11,"pdf_bytes":pdf.stat().st_size,"zip_bytes":archive.stat().st_size}))
