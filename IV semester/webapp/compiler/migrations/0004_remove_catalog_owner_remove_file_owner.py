# Generated by Django 4.2 on 2023-05-03 14:46

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('compiler', '0003_rename_myuser_user'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='catalog',
            name='owner',
        ),
        migrations.RemoveField(
            model_name='file',
            name='owner',
        ),
    ]
