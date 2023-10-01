from django.urls import path
from django.views.decorators.csrf import csrf_exempt

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('compiler/register/', views.register, name='register'),
    path('compiler/loginview/', views.loginview, name='loginview'),
    path('compiler/logoutview/', views.logoutview, name='logoutview'),
    path('compiler/usernamevalidation/', csrf_exempt(views.usernamevalidation), name='usernamevalidation'),
    path('compiler/loginvalidation/', csrf_exempt(views.loginvalidation), name='loginvalidation'),
    path('compiler/adddirectory/', views.adddirectory, name='adddirectory'),
    path('compiler/addfile/', views.addfile, name='addfile'),
    path('compiler/deletedirectory/', views.deletedirectory, name='deletedirectory'),
    path('compiler/deletefile/', views.deletefile, name='deletefile'),
    path('compiler/getfile/<int:file_id>/', views.getfile, name='getfile'),
    path('compiler/savefile/<int:file_id>/', views.savefile, name='savefile'),
    path('compiler/compilefile/<int:file_id>/<str:std>/<str:proc>/<str:proc_flag>/<str:flags>/<int:save>/',
         views.compilefile, name='compilefile'),
]
