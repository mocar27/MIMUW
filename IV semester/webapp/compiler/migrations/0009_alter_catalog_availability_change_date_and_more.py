# Generated by Django 4.2 on 2023-05-03 16:50

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
        ('compiler', '0008_alter_catalog_availability_change_date_and_more'),
    ]

    operations = [
        migrations.AlterField(
            model_name='catalog',
            name='availability_change_date',
            field=models.DateTimeField(default=models.DateTimeField(default=django.utils.timezone.now)),
        ),
        migrations.AlterField(
            model_name='catalog',
            name='creation_date',
            field=models.DateTimeField(default=django.utils.timezone.now),
        ),
        migrations.AlterField(
            model_name='catalog',
            name='last_changed',
            field=models.DateTimeField(default=models.DateTimeField(default=django.utils.timezone.now)),
        ),
        migrations.AlterField(
            model_name='file',
            name='availability_change_date',
            field=models.DateTimeField(default=models.DateTimeField(default=django.utils.timezone.now)),
        ),
        migrations.AlterField(
            model_name='file',
            name='creation_date',
            field=models.DateTimeField(default=django.utils.timezone.now),
        ),
        migrations.AlterField(
            model_name='file',
            name='last_changed',
            field=models.DateTimeField(default=models.DateTimeField(default=django.utils.timezone.now)),
        ),
        migrations.AlterField(
            model_name='section',
            name='creation_date',
            field=models.DateTimeField(default=django.utils.timezone.now),
        ),
    ]
