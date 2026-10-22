#!/usr/bin/env python3
"""Create 11 complete English printable lesson sheets and a consolidated PDF/ZIP."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageOps
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import landscape, A4
import shutil, zipfile, json

ROOT=Path("parenting/everyday-parenting")
PRINT=ROOT/"print"; ORIGINAL=ROOT/"original"
PRINT.mkdir(parents=True,exist_ok=True)
ORIGINAL.mkdir(parents=True,exist_ok=True)
ITEMS=[
("01_Praise","PRAISE"),("02_Antecedents","ANTECEDENTS"),
("03_Shaping","SHAPING"),("04_Simulation","SIMULATION"),
("05_Modeling","MODELING"),("06_Positive_Opposite","POSITIVE OPPOSITE"),
("07_Point_Programs","POINT PROGRAMS"),("08_Attending","ATTENDING"),
("09_Planned_Ignoring","PLANNED IGNORING"),("10_Punishment","PUNISHMENT BASICS"),
("11_Time_Out","TIME OUT")]
STEPS={
"06_Positive_Opposite":[
("Notice a problem","Select a behavior you want to reduce."),
("Name the opposite","Decide what good behavior should replace it."),
("Praise that behavior","Catch and praise the positive opposite.")],
"07_Point_Programs":[
("Set a goal","Choose a specific behavior."),
("Choose tokens","Pick points, stars or stickers."),
("Track progress","Record points earned and spent."),
("Set earnings","Assign points to each behavior."),
("Offer a menu","Price appealing rewards in points.")],
"08_Attending":[
("Choose a behavior","Select what you want to increase."),
("Choose attention","Praise, smile, talk or show affection."),
("Respond immediately","Give attention while the good behavior happens."),
("Do it often","Notice the desirable behavior frequently.")],
"10_Punishment":[
("Specify the problem","Identify the behavior you want to reduce."),
("Define the alternative","Choose its positive opposite."),
("Reward the good","Praise the replacement behavior often."),
("Make it brief","Use only mild, brief consequences if needed."),
("Keep your calm","Avoid harshness and repeated arguments.")]}
COMPLETE={x for x,_ in ITEMS if x not in STEPS}
FONT="/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
BOLD="/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
def ft(n,b=False): return ImageFont.truetype(BOLD if b else FONT,n)
def wrapped(d,text,width,f):
    lines=[]; line=""
    for word in text.split():
        candidate=(line+" "+word).strip()
        if line and d.textbbox((0,0),candidate,font=f)[2]>width:
            lines.append(line);line=word
        else:line=candidate
    if line:lines.append(line)
    return lines
def centered(d,text,x,y,width,n,b=False,fill=(31,56,64)):
    f=ft(n,b)
    for ln in wrapped(d,text,width,f):
        tw=d.textbbox((0,0),ln,font=f)[2]
        d.text((int(x-tw/2),int(y)),ln,font=f,fill=fill)
        y+=n+10
    return y
def mascot(draw,cx,cy,scale=1):
    # Deliberately original mascot drawing for the newly reconstructed cards.
    s=scale
    ell=lambda coords,fill:draw.ellipse(tuple(int(v) for v in coords),fill=fill)
    ell((cx-47*s,cy+4*s,cx+47*s,cy+112*s),(255,228,126))
    ell((cx-61*s,cy-109*s,cx+61*s,cy+13*s),(54,203,192))
    draw.line([(int(cx-28*s),int(cy-105*s)),(int(cx-42*s),int(cy-139*s))],fill=(23,139,144),width=max(3,int(6*s)))
    draw.line([(int(cx+28*s),int(cy-105*s)),(int(cx+42*s),int(cy-139*s))],fill=(23,139,144),width=max(3,int(6*s)))
    ell((cx-52*s,cy-153*s,cx-33*s,cy-134*s),(18,166,170))
    ell((cx+33*s,cy-153*s,cx+52*s,cy-134*s),(18,166,170))
    for ex in (-21,21):
        ell((cx+(ex-14)*s,cy-68*s,cx+(ex+14)*s,cy-37*s),"white")
        ell((cx+(ex-4)*s,cy-57*s,cx+(ex+5)*s,cy-44*s),(24,66,83))
    draw.arc(tuple(int(v) for v in (cx-20*s,cy-40*s,cx+20*s,cy-8*s)),0,180,fill=(32,82,91),width=max(3,int(4*s)))
    tx=cx+84*s;ty=cy+76*s
    ell((tx-20*s,ty-21*s,tx+20*s,ty+46*s),(157,111,214))
    ell((tx-33*s,ty-88*s,tx+33*s,ty-21*s),(188,148,230))
    for ex in (-12,12):
        ell((tx+(ex-9)*s,ty-69*s,tx+(ex+9)*s,ty-48*s),"white")
        ell((tx+(ex-2)*s,ty-62*s,tx+(ex+5)*s,ty-51*s),(67,40,93))
    draw.arc(tuple(int(v) for v in (tx-14*s,ty-45*s,tx+14*s,ty-28*s)),0,180,fill=(72,40,84),width=max(2,int(3*s)))
def reconstructed(name,title):
    W,H=2048,1300
    im=Image.new("RGB",(W,H),(251,253,252));d=ImageDraw.Draw(im)
    d.rounded_rectangle((40,38,W-40,H-38),radius=42,fill="white",outline=(225,234,236),width=4)
    d.text((92,76),title,font=ft(76,True),fill=(25,58,66))
    d.text((94,173),"ENGLISH COURSE SUMMARY  |  RECONSTRUCTED, NOT ORIGINAL YALE ARTWORK",
           font=ft(24,True),fill=(98,116,122))
    d.line((92,228,W-92,228),fill=(224,235,235),width=3)
    steps=STEPS[name];n=len(steps);cw=(W-172)/n
    for i,(heading,sub) in enumerate(steps):
        l=86+i*cw;r=l+cw-16;cx=(l+r)/2
        d.rounded_rectangle((int(l),271,int(r),1164),radius=32,
            fill=(241,250,249) if i%2==0 else (249,246,252),
            outline=(228,238,238),width=2)
        accent=(13,160,159) if i%2==0 else (131,87,190)
        d.ellipse((cx-45,299,cx+45,389),fill=accent)
        centered(d,str(i+1),cx,313,cw-24,46,True,fill="white")
        mascot(d,cx-39,721,1.00 if n<=4 else .78)
        d.line((int(l+40),916,int(r-40),916),fill=(212,225,225),width=3)
        y=centered(d,heading,cx,941,cw-46,34 if n<=4 else 28,True)
        centered(d,sub,cx,y+16,cw-56,26 if n<=4 else 23)
    d.text((94,1210),"Course-aligned adaptation for home printing. Not an official course graphic.",
           font=ft(24),fill=(99,115,122))
    im.save(PRINT/(name+".png"),optimize=True)
def original(name):
    src=ROOT/"complete_candidates"/"03_Shaping.png" if name=="03_Shaping" else ROOT/(name+".png")
    if not src.is_file():raise FileNotFoundError(src)
    with Image.open(src) as im:
        resized=ImageOps.contain(im.convert("RGB"),(1940,1180),Image.Resampling.LANCZOS)
        bg=Image.new("RGB",(2048,1300),"white")
        bg.paste(resized,((2048-resized.width)//2,(1300-resized.height)//2))
        bg.save(PRINT/(name+".png"),optimize=True)
for name,title in ITEMS:
    src=ROOT/(name+".png")
    if not src.exists():raise FileNotFoundError(src)
    shutil.copy2(src,ORIGINAL/(name+".png"))
    if name in COMPLETE:original(name)
    else:reconstructed(name,title)
pdf=ROOT/"Everyday_Parenting_A4.pdf"
c=canvas.Canvas(str(pdf),pagesize=landscape(A4),pageCompression=1)
w,h=landscape(A4)
for name,_ in ITEMS:
    with Image.open(PRINT/(name+".png")) as im: iw,ih=im.size
    s=min((w-12)/iw,(h-12)/ih)
    c.drawImage(str(PRINT/(name+".png")),(w-iw*s)/2,(h-ih*s)/2,width=iw*s,height=ih*s)
    c.showPage()
c.setTitle("Everyday Parenting | 11 complete English step summaries")
c.save()
readme=ROOT/"PRINT_README.md"
readme.write_text("""# Complete English 11-card printable study collection

This collection has eleven cards with complete step-by-step English information.

Seven cards preserve the complete source teaching illustrations: Praise,
Antecedents, Shaping, Simulation, Modeling, Planned Ignoring, and Time Out.
The higher-resolution Shaping diagram is from the Palo Alto University
parenting infographic hosted by MVWSD.

Four cards are newly reconstructed from course teaching points:
Positive Opposite (3 steps), Point Programs (5 steps), Attending (4 steps),
and Punishment Basics (five course-aligned practical guidance items).
These FOUR are editorial adaptations, **not original Yale full-slide screenshots**.
The Punishment card especially is a practical summary rather than a claim
to reproduce the video lesson's exact order of five headings.

The folder named original contains the eleven previous untouched video frames
(including five incomplete stills). The print folder contains eleven
complete printable cards. The PDF is eleven A4 landscape pages.

Images captured from videos are only 640x360 source resolution, and resizing
does not improve true detail. Reconstruction typography is high-resolution.

Source course: https://www.coursera.org/learn/everyday-parenting
""",encoding="utf-8")
zfile=ROOT/"Everyday_Parenting_Print.zip"
with zipfile.ZipFile(zfile,"w",zipfile.ZIP_DEFLATED,compresslevel=7) as z:
    z.write(pdf,pdf.name);z.write(readme,readme.name)
    for folder in (ORIGINAL,PRINT):
        for p in sorted(folder.glob("*.png")):
            z.write(p,p.relative_to(ROOT))
assert len(list(PRINT.glob("*.png")))==11
assert pdf.read_bytes().startswith(b"%PDF-")
print(json.dumps({"complete":11,"source_graphics":7,
    "clearly_labeled_reconstructed_graphics":4,
    "pdf_bytes":pdf.stat().st_size,"zip_bytes":zfile.stat().st_size}))
