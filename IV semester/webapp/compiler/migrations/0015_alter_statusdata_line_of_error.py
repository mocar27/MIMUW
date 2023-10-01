# Generated by Django 4.1.7 on 2023-05-08 12:27

import django.core.validators
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('compiler', '0014_alter_statusdata_line_of_error'),
    ]

    operations = [
        migrations.AlterField(
            model_name='statusdata',
            name='line_of_error',
            field=models.PositiveIntegerField(validators=[django.core.validators.MinValueValidator(0)]),
        ),
    ]