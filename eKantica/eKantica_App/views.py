from django.http.response import HttpResponse
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt

# Create your views here.

def home(request):
    return render(request, "Home.html")

@csrf_exempt
def mojakantica(request):
    context = {}
    #ovdje će se odvijati obrada podataka 

    if request.method == "GET":

        print("registriram GET")

        http_request = {}
        for key in request.META:
            if "HTTP" in key:
                http_request.update({key:request.META[key]})
        print(http_request)
        #npr ako je poslano kljuc:vrijednost, u request.META to izgleda kao HTTP_KLJUC:vrijednost

    if request.method == "POST":
        print("registriram POST")
        print(request.POST)

    context["temp"] = 0
    context["moisture"] = 0
    context["humidity"] = 0
    context["water"] = 0

    return render(request, "MojaKantica.html", context)

def info(request):
    return render(request, "Info.html")