from django.http.response import HttpResponse
from django.shortcuts import render

# Create your views here.

def home(request):
    return render(request, "Home.html")

def mojakantica(request):
    return render(request, "MojaKantica.html")

def info(request):
    return render(request, "Info.html")