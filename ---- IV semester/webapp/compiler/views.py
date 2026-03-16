import os
import subprocess
import json
from django.contrib import auth
from django.http import HttpResponseRedirect, JsonResponse, HttpResponseNotFound
from django.shortcuts import render
from .models import *
from django.urls import reverse
from django.contrib.auth import logout
from django.contrib.auth.decorators import login_required
from .forms import *


@login_required(login_url='/compiler/loginview/')
def index(request):
    catalogs = Catalog.objects.filter(owner=request.user, availability=True)
    files = File.objects.filter(owner=request.user, availability=True)
    context = {
        'catalogs': catalogs,
        'files': files
    }
    return render(request, 'compiler/index.html', context)


def register(request):
    if request.method == 'POST':
        form = RegisterForm(request.POST)
        new_user = User.objects.create_user(username=form.data['username'], email=form.data['login'])
        new_user.set_password(form.data['password'])
        new_user.save()
        return HttpResponseRedirect(reverse('index', args=()))
    else:
        form = RegisterForm()
        return render(request, 'compiler/register.html', {'form': form})


def loginview(request):
    if request.method == 'POST':
        form = LoginForm(request.POST)
        if form.is_valid():
            user = auth.authenticate(username=form.data['username'], password=form.data['password'])
            if user:
                auth.login(request, user)
                return HttpResponseRedirect(reverse('index', args=()))
            else:
                form = LoginForm()
                return render(request, 'compiler/login.html', {'form': form,
                                                               'error': 'User does not exist in the database'})
        return render(request, 'compiler/login.html', {'form': form, 'error': 'Invalid username or password'})
    else:
        form = LoginForm()
        return render(request, 'compiler/login.html', {'form': form})


def logoutview(request):
    logout(request)
    return render(request, 'compiler/logout.html', {})


def usernamevalidation(request):
    if request.method == 'POST':
        data = json.loads(request.body)
        username = data['username']

        if not str(username).isalnum():
            return JsonResponse({'username_error': 'Username must contain only letters and digits'}, status=400)

        if User.objects.filter(username=username).exists():
            return JsonResponse({'username_error': 'Username already exists'}, status=409)
        return JsonResponse({'username_valid': True})


def loginvalidation(request):
    if request.method == 'POST':
        data = json.loads(request.body)
        loginval = data['login']

        if not str(loginval).isalnum():
            return JsonResponse({'login_error': 'Login must contain only letters and digits'}, status=400)

        if User.objects.filter(email=loginval).exists():
            return JsonResponse({'login_error': 'Login already exists'}, status=409)
        return JsonResponse({'login_valid': True})


@login_required(login_url='/compiler/loginview/')
def adddirectory(request):
    try:
        catalogs = Catalog.objects.filter(availability=True, owner=request.user)
    except Catalog.DoesNotExist:
        return HttpResponseNotFound()

    if request.method == 'POST':
        form = CatalogForm(request.POST)
        if form.is_valid():
            new_catalog = form.save()
            parent_catalog_id = request.POST.get('parent_catalog')
            if parent_catalog_id:
                try:
                    parent_catalog = Catalog.objects.get(id=parent_catalog_id)
                    new_catalog.parent_catalog = parent_catalog
                    parent_catalog.last_changed = timezone.now()
                    parent_catalog.save()
                except Catalog.DoesNotExist:
                    return HttpResponseNotFound()
            new_catalog.owner = request.user
            new_catalog.save()
        return HttpResponseRedirect(reverse('index', args=()))
    else:
        context = {
            'catalogs': catalogs,
            'form': CatalogForm(),
        }
        return render(request, 'compiler/adddirectory.html', context)


@login_required(login_url='/compiler/loginview/')
def addfile(request):
    try:
        catalogs = Catalog.objects.filter(availability=True, owner=request.user)
    except Catalog.DoesNotExist:
        return HttpResponseNotFound()

    if request.method == 'POST':
        form = FileForm(request.POST)
        if form.is_valid():
            new_file = form.save()
            catalog_id = request.POST.get('catalog')
            if catalog_id:
                try:
                    catalog = Catalog.objects.get(id=catalog_id)
                    new_file.catalog = catalog
                    catalog.last_changed = timezone.now()
                    catalog.save()
                except Catalog.DoesNotExist:
                    return HttpResponseNotFound()
            new_file.owner = request.user
            new_file.save()
        return HttpResponseRedirect(reverse('index', args=()))
    else:
        context = {
            'catalogs': catalogs,
            'form': FileForm(),
        }
        return render(request, 'compiler/addfile.html', context)


@login_required(login_url='/compiler/loginview/')
def deletedirectory(request):
    try:
        catalogs = Catalog.objects.filter(availability=True, owner=request.user)
    except Catalog.DoesNotExist:
        return HttpResponseNotFound()
    context = {
        'catalogs': catalogs
    }

    if request.method == 'POST':
        catalog_id = request.POST.get('catalog', None)
        if catalog_id:
            try:
                catalog = Catalog.objects.get(id=catalog_id)
            except Catalog.DoesNotExist:
                return HttpResponseNotFound()
            catalog.availability = False
            kid_catalogs = Catalog.objects.filter(owner=request.user, availability=True, parent_catalog=catalog)

            for cat in kid_catalogs:
                for file in File.objects.filter(availability=True, catalog=cat):
                    file.availability = False
                    file.availability_change_date = timezone.now()
                    file.last_changed = timezone.now()
                    file.save()
                cat.availability = False
                cat.availability_change_date = timezone.now()
                cat.last_changed = timezone.now()
                cat.save()

            for file in File.objects.filter(availability=True, catalog=catalog):
                file.availability = False
                file.availability_change_date = timezone.now()
                file.last_changed = timezone.now()
                file.save()

            catalog.availability_change_date = timezone.now()
            catalog.last_changed = timezone.now()

            if catalog.parent_catalog:
                catalog.parent_catalog.last_changed = timezone.now()
                catalog.parent_catalog.save()
            catalog.save()
        return HttpResponseRedirect(reverse('index', args=()))
    else:
        return render(request, 'compiler/deletedirectory.html', context)


@login_required(login_url='/compiler/loginview/')
def deletefile(request):
    try:
        files = File.objects.filter(availability=True, owner=request.user)
    except File.DoesNotExist:
        return HttpResponseNotFound()
    context = {
        'files': files
    }

    if request.method == 'POST':
        file_id = request.POST.get('file', None)
        if file_id:
            try:
                file = File.objects.get(id=file_id)
            except File.DoesNotExist:
                return HttpResponseNotFound()
            file.availability = False
            file.availability_change_date = timezone.now()
            file.last_changed = timezone.now()
            if file.catalog:
                file.catalog.last_changed = timezone.now()
                file.catalog.save()
            file.save()
        return HttpResponseRedirect(reverse('index', args=()))
    else:
        return render(request, 'compiler/deletefile.html', context)


@login_required(login_url='/compiler/loginview/')
def getfile(request, file_id):
    try:
        file = File.objects.get(id=file_id, availability=True, owner=request.user)
    except File.DoesNotExist:
        return JsonResponse({"result": "File does not exist"}, status=409)
    return JsonResponse({'data': file.source_code})


@login_required(login_url='/compiler/loginview/')
def savefile(request, file_id):
    try:
        file = File.objects.get(id=file_id, availability=True, owner=request.user)
    except File.DoesNotExist:
        return JsonResponse({"result": "File does not exist"}, status=409)
    file_content = request.POST.get('file-content', 'error')
    file.source_code = file_content
    file.last_changed = timezone.now()
    file.save()
    return JsonResponse({'success': True, 'data': file.source_code})


@login_required(login_url='/compiler/loginview/')
def compilefile(request, file_id, std, proc, proc_flag, flags, save):
    try:
        file = File.objects.get(id=file_id, availability=True, owner=request.user)
    except File.DoesNotExist:
        return JsonResponse({"result": "File not found"}, status=409)

    with open("temp.c", "w") as f:
        f.write(file.source_code)

    compilation = ['sdcc', '-S']
    if std != ' ':
        compilation.append(std)
    if proc != ' ':
        compilation.append(proc)
    if proc_flag != ' ':
        compilation.append(proc_flag)
    if flags != ' ':
        for flag in flags.split():
            compilation.append(flag)

    result = subprocess.run(compilation + ["temp.c"], stderr=subprocess.PIPE)

    if result.returncode != 0:
        return JsonResponse({"result": result.stderr.decode('utf-8')})

    with open("temp.asm", "r") as f:
        result = f.read()

    os.remove("temp.c")

    if save == 0:
        os.remove("temp.asm")

    return JsonResponse({"result": result})
