"""Dibujos técnicos (vista superior) de cada robot, en SVG.

Las medidas salen de la documentación de cada repositorio: barra curva de radio
95 mm a 4,8° para Deimonyag, paso de 8 mm para UNICUM y celdas de laberinto para
los micromouse. Los usa build_site.py en la portada y en el hero de las fichas.
"""
import math
W,H=240,280
def grid():
    s=['<g class="grid">']
    for x in range(0,W+1,20): s.append(f'<line x1="{x}" y1="0" x2="{x}" y2="{H}"/>')
    for y in range(0,H+1,20): s.append(f'<line x1="0" y1="{y}" x2="{W}" y2="{y}"/>')
    s.append('</g>'); return ''.join(s)
def svg(name,body,label):
    return f'<svg class="draw" viewBox="0 0 {W} {H}" role="img" aria-label="{label}">{grid()}{body}</svg>'
def sensors(pts,r=3.2):
    return ''.join(f'<circle class="sensor" style="--i:{i}" cx="{x:.1f}" cy="{y:.1f}" r="{r}"/>' for i,(x,y) in enumerate(pts))

# Deimonyag: barra curva R95mm, 12 sensores a 4.8 grados, EDF27
k=1.35; R=95*k; cy0=58; cx=120; ccy=cy0+R
pts=[]
for i in range(12):
    t=math.radians(-26.4+4.8*i); pts.append((cx+R*math.sin(t), ccy-R*math.cos(t)))
a0,a1=math.radians(-30),math.radians(30)
p0=(cx+R*math.sin(a0),ccy-R*math.cos(a0)); p1=(cx+R*math.sin(a1),ccy-R*math.cos(a1))
Ri=R-9; q0=(cx+Ri*math.sin(a0),ccy-Ri*math.cos(a0)); q1=(cx+Ri*math.sin(a1),ccy-Ri*math.cos(a1))
Ro=R+9; o0=(cx+Ro*math.sin(a0),ccy-Ro*math.cos(a0)); o1=(cx+Ro*math.sin(a1),ccy-Ro*math.cos(a1))
d=(f'<path class="track" d="M150 290 C150 230 120 200 118 120 C116 60 90 30 40 -10"/>'
   f'<path class="body" d="M{o0[0]:.1f} {o0[1]:.1f} A{Ro:.1f} {Ro:.1f} 0 0 1 {o1[0]:.1f} {o1[1]:.1f} L{q1[0]:.1f} {q1[1]:.1f} A{Ri:.1f} {Ri:.1f} 0 0 0 {q0[0]:.1f} {q0[1]:.1f} Z"/>'
   '<path class="soft" d="M112 160 L112 76 M128 160 L128 76"/>'
   '<rect class="body" x="72" y="160" width="96" height="86" rx="6"/>'
   '<rect class="body" x="54" y="186" width="16" height="40" rx="3"/><rect class="body" x="170" y="186" width="16" height="40" rx="3"/>'
   '<circle class="body" cx="120" cy="206" r="25"/><circle class="soft" cx="120" cy="206" r="17"/><circle class="soft" cx="120" cy="206" r="5"/>'
   '<rect class="soft" x="84" y="168" width="20" height="14"/>'
   + sensors(pts))
deim=svg('deimonyag',d,'Vista superior de Deimonyag: barra curva de 12 sensores, turbina EDF y dos motores')

# UNICUM: barra recta 12 sensores paso 8 mm, N20
k=1.3; x0=120-44*k
pts=[(x0+i*8*k,64) for i in range(12)]
d=('<path class="track" d="M80 290 C80 220 120 200 120 140 C120 80 120 40 120 -10"/>'
   f'<rect class="body" x="{x0-8:.1f}" y="54" width="{88*k+16:.1f}" height="20" rx="3"/>'
   '<path class="soft" d="M96 74 L104 168 M144 74 L136 168"/>'
   '<rect class="body" x="84" y="168" width="72" height="72" rx="5"/>'
   '<rect class="body" x="66" y="190" width="16" height="34" rx="3"/><rect class="body" x="158" y="190" width="16" height="34" rx="3"/>'
   '<rect class="soft" x="96" y="182" width="22" height="14"/><rect class="soft" x="124" y="182" width="20" height="26"/>'
   '<rect class="soft" x="98" y="214" width="44" height="18" rx="2"/>'
   + sensors(pts))
unic=svg('unicum',d,'Vista superior de UNICUM: barra recta de 12 sensores y dos motores N20')

def cell(open_front=True):
    s='<path class="wall" d="M30 250 L30 30 M210 30 L210 250"/>'
    s+='<path class="wall" d="M30 30 L90 30 M150 30 L210 30"/>' if open_front else '<path class="wall" d="M30 30 L210 30"/>'
    s+='<path class="soft" d="M30 250 L210 250" stroke-dasharray="3 5"/>'
    return s
# LEA_PAREDES: 2 frontales, 2 laterales, turbina 30 mm, ruedas 26 mm
d=(cell(True)+
   '<path class="body" d="M86 112 Q86 92 104 88 L136 88 Q154 92 154 112 L154 214 Q154 226 142 226 L98 226 Q86 226 86 214 Z"/>'
   '<rect class="body" x="72" y="168" width="12" height="36" rx="3"/><rect class="body" x="156" y="168" width="12" height="36" rx="3"/>'
   '<circle class="body" cx="120" cy="160" r="20"/><circle class="soft" cx="120" cy="160" r="13"/><circle class="soft" cx="120" cy="160" r="4"/>'
   '<path class="beam" d="M106 90 L98 30 M134 90 L142 30 M90 120 L30 120 M150 120 L210 120"/>'
   + sensors([(106,92),(134,92),(89,120),(151,120)],3.6))
lea=svg('lea',d,'Vista superior de LEA_PAREDES dentro de una celda: cuatro sensores infrarrojos y turbina de 30 mm')

# AUS_KIM V2: Sharp frontal + laterales, QRE piso, N20 con encoder
d=(cell(False)+
   '<path class="body" d="M78 104 L162 104 L170 118 L170 222 Q170 232 160 232 L80 232 Q70 232 70 222 L70 118 Z"/>'
   '<rect class="body" x="58" y="168" width="11" height="40" rx="3"/><rect class="body" x="171" y="168" width="11" height="40" rx="3"/>'
   '<rect class="soft" x="98" y="140" width="44" height="74" rx="2"/>'
   '<rect class="soft" x="110" y="96" width="20" height="10"/><rect class="soft" x="72" y="124" width="10" height="20"/><rect class="soft" x="158" y="124" width="10" height="20"/>'
   '<path class="beam" d="M120 96 L120 30 M72 134 L30 134 M168 134 L210 134"/>'
   '<circle class="soft" cx="120" cy="122" r="5"/>'
   + sensors([(120,100),(76,134),(164,134)],3.6))
aus=svg('aus',d,'Vista superior de AUS_KIM V2 dentro de una celda: tres sensores Sharp, sensor de piso y motores N20')
DRAWINGS = {'deimonyag': deim, 'unicum': unic, 'lea-paredes': lea, 'aus-kim': aus}
