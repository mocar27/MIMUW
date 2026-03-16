from django import forms
from .models import Catalog, File


class CatalogForm(forms.ModelForm):
    class Meta:
        model = Catalog
        fields = ['name', 'description']
        labels = {
            'name': 'Nazwa:',
            'description': 'Opis:',
        }


class FileForm(forms.ModelForm):
    class Meta:
        model = File
        fields = ['name', 'source_code', 'description']
        labels = {
            'name': 'Nazwa:',
            'source_code': 'Kod źródłowy:',
            'description': 'Opis:',
        }


class RegisterForm(forms.Form):
    username = forms.CharField(label='Nazwa użytkownika:', max_length=100)
    login = forms.CharField(label='Login:', max_length=100)
    password = forms.CharField(label='Hasło:')


class LoginForm(forms.Form):
    username = forms.CharField(label='Nazwa użytkownika:', max_length=100)
    password = forms.CharField(label='Hasło:')
