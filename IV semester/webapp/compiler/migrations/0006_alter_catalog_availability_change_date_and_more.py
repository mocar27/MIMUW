# Generated by Django 4.2 on 2023-05-03 16:48

import datetime
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('compiler', '0005_alter_catalog_availability_change_date_and_more'),
    ]

    operations = [
        migrations.AlterField(
            model_name='catalog',
            name='availability_change_date',
            field=models.DateTimeField(default=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 867575))),
        ),
        migrations.AlterField(
            model_name='catalog',
            name='creation_date',
            field=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 867575)),
        ),
        migrations.AlterField(
            model_name='catalog',
            name='last_changed',
            field=models.DateTimeField(default=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 867575))),
        ),
        migrations.AlterField(
            model_name='file',
            name='availability_change_date',
            field=models.DateTimeField(default=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 868573))),
        ),
        migrations.AlterField(
            model_name='file',
            name='creation_date',
            field=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 868573)),
        ),
        migrations.AlterField(
            model_name='file',
            name='last_changed',
            field=models.DateTimeField(default=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 868573))),
        ),
        migrations.AlterField(
            model_name='section',
            name='creation_date',
            field=models.DateTimeField(default=datetime.datetime(2023, 5, 3, 18, 48, 8, 871564)),
        ),
    ]
