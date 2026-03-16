from django.contrib import admin

from .models import *

admin.site.register(Catalog)
admin.site.register(File)
admin.site.register(SectionStatus)
admin.site.register(SectionType)
admin.site.register(Section)
