#Kacper Kulczak 279079
#Program do zadania 2.12 na pracownie z przedmiotu Analiza Numeryczna M

using Plotly;
include("interps.jl")


#interpolacja dwoma wielomianami Newtona rybki
ryba_x1 =[3.,13.,26.,38.,49.,65.,77.,93.,106.,123.,131.,138.,144.,149.,155.]
ryba_y1 = [81.,72.,65.,60.,57.,55.,56.,58.,63.,71.,75.,81.,86.,92.,99.]
ryba_x2 =[3.,13.,26.,38.,49.,65.,77.,93.,106.,123.,131.,138.,144.,149.,155.]
ryba_y2 =[81.,90.,97.,102.,105.,107.,106.,104.,99.,91.,87.,81.,76.,70.,63.]
Newton1 = interpNewton(ryba_x1,ryba_y1)
Newton2 = interpNewton(ryba_x2,ryba_y2)
xss = linspace(ryba_x1[1], ryba_x1[end], 1000)
trace1 = scatter(;x = xss, y = map(Newton1,xss) , mode = "lines" ,name = "wielomian 1")
trace2 = scatter(;x = xss, y = map(Newton2,xss) , mode = "lines" ,name = "wielomian 2")
trace3 = scatter(;x=ryba_x1, y=ryba_y1, mode = "markers", name = "wezły interpolacyjne 1")
trace4 = scatter(;x=ryba_x2, y=ryba_y2, mode = "markers", name = "węzy interpolacyjne 2")
layout = Layout(;title="Interpolacja wielomianami interpolacyjnymi Newtona",
    xaxis=attr(title="", ),
    yaxis=attr(title="",))
plot([trace1,trace2,trace3,trace4],layout)

#interpolacja dwoma funckjami sklejanymi rybki
Spline1 = interpspline(ryba_x1,ryba_y1)
Spline2 = interpspline(ryba_x2,ryba_y2)
xss = linspace(ryba_x1[1], ryba_x1[end], 1000)
trace1 = scatter(;x = xss, y = map(Spline1,xss) , mode = "lines", name="funkcja 1")
trace2 = scatter(;x = xss, y = map(Spline2,xss) , mode = "lines", name="funckja 2")
trace3 = scatter(;x=ryba_x1, y=ryba_y1, mode = "markers", name="węzły interpolacyjne")
trace4 = scatter(;x=ryba_x2, y=ryba_y2, mode = "markers", name ="węzły interpolacyjne")
layout = Layout(;title="Interpolacja naturalnymi funkcjami sklejanymi",
    xaxis=attr(title="", ),
    yaxis=attr(title="",))
plot([trace1,trace2,trace3,trace4],layout)


#rybka za pomocą interpolowanej krzywej parametrycznej
nryba_y1 = myreverse(ryba_y1)
pop!(nryba_y1)
nryba_x1 = myreverse(ryba_x1)
pop!(nryba_x1)
rybap_x = append!(nryba_x1 , ryba_x2 )
rybap_y = append!(nryba_y1, ryba_y2)
a = 1
b = length(rybap_x)
t = linspace(a, b, b)
Splinex = interpspline(t,rybap_x)
Spliney = interpspline(t,rybap_y)
xss = linspace(a, b, 1000)
trace1 = scatter(;x = map(Splinex,xss), y = map(Spliney,xss), mode = "lines",name= "funkcja")
trace3 = scatter(;x=rybap_x, y=rybap_y, mode="markers",name = "węzły interpolacji")
layout = Layout(;title = "Interpolacja krzywej parametrycznej naturalną funckją sklejaną")
plot([trace1,trace3], layout)
plot([trace1,trace3], layout)

#mapa Polski z punktow ze list na pracowni
tajem_xs, tajem_ys = ([3.7,3.2,2.7,2.1,1.7,1.1,0.7,0.4,0.4,0.5,0.3,0.6,0.6,0.7,0.6,0.8,0.8,0.6,0.8,0.9,1.1,1.4,1.8,1.7,1.9,2.2,2.1,2.7,2.6,3.3,3.5,3.7,3.9,4.2,4.3,4.5,4.7,5.0,5.5,5.9,6.2,6.4,6.3,6.5,6.8,7.2,7.1,7.2,6.8,6.7,6.8,6.4,6.2,6.9,6.8,6.6,6.5,6.1,5.5,5.0,4.6,4.1,3.7,3.4,3.2,3.7],[6.4,6.7,6.5,6.4,6.0,5.9,5.7,5.7,5.4,5.0,4.6,4.3,4.0,3.7,3.2,2.9,2.6,2.4,2.3,2.4,2.2,2.1,2.0,1.8,1.4,1.5,1.8,1.6,1.4,1.3,0.9,0.6,0.8,0.7,0.4,0.5,0.7,0.6,0.8,0.6,0.4,0.3,0.7,1.2,1.7,2.0,2.2,2.4,2.8,3.2,3.6,3.9,4.2,4.5,5.1,5.6,6.0,6.2,6.1,6.2,6.2,6.3,6.0,6.1,6.5,6.4])
tempxs, tempys  =interpparametric(tajem_xs, tajem_ys)
tajem_trace0 = scatter(; x=tempxs ,  y=tempys , mode = "lines", name = "Interpolowana linia")
trace1= scatter(;x = tajem_xs, y = tajem_ys, mode = "markers", name = "Dane punkty")
plot([tajem_trace0,trace1])

#spirala
a=0
b=10*pi
aa,bb,cc =make_param_curve(t->t*cos(t),t->t*sin(t),linspace(a,b,40) ,linspace(a,b,4000), "")

#Cykloida wydluzona
a=-3*pi
b=3*pi
aa,bb,cc =make_param_curve(t->1/3*t - sin(t),t->1/3 -cos(t),linspace(a,b,40) ,linspace(a,b,4000), "Cykloida wydłużona")

#Krzywa z petlami
a=-2*pi
b=2*pi
aa,bb,cc =make_param_curve(t->t+sin(2*t),t->t+sin(3*t),linspace(a,b,40) ,linspace(a,b,4000), "Krzywa z pętlami")

#Globus
a=0
b=9*pi
aa,bb,cc =make_param_curve(t->t*cos(t),t->sin(t),linspace(a,b,40) ,linspace(a,b,4000), "Globus")

#Inna krzywa
a=0
<<<<<<< HEAD
b=15*pi
aa,bb,cc =make_param_curve(t->t*cos(t),t->t,linspace(a,b,40) ,linspace(a,b,4000), "")
=======
b=10*3.14
make_param_curve(t->t*cos(t),t->t*sin(t),linspace(a,b,50) ,linspace(a,b,4000), "")  

>>>>>>> 31480e236ad47585b5bccec604fa1446572e7ba5
