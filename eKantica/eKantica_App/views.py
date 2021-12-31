from django.http.response import HttpResponse
from django.shortcuts import render

# Create your views here.

def home(request):
    return render(request, "Home.html")

def mojakantica(request):
    #ovdje će se odvijati obrada podataka 
    if request.method == "POST":
        if "ime_post_metode1" in request.POST:
            pass
        pass

    return render(request, "MojaKantica.html")

def info(request):
    return render(request, "Info.html")