# Generated by Django 4.2 on 2023-05-03 17:02

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('compiler', '0010_alter_catalog_availability_change_date_and_more'),
    ]

    operations = [
        migrations.AlterField(
            model_name='file',
            name='catalog',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='compiler.catalog'),
        ),
    ]
