from django.contrib import admin
from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name="home"),
    path('mojakantica', views.mojakantica, name="mojakantica"),
    path('info', views.info, name="info")
]
