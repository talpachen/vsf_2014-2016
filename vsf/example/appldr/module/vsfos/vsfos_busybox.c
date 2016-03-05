/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "vsf.h"
#include "vsfos.h"

#define VSFOSCFG_HANDLER_NUM				20

struct vsfos_ctx_t
{
	struct vsfile_t *curfile;
	uint8_t user_buff[64];
};

static vsf_err_t vsfos_busybox_help(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;
	struct vsfos_ctx_t *ctx = (struct vsfos_ctx_t *)param->context;
	struct vsfos_busybox_help_t
	{
		struct vsfshell_handler_t *handler;
	} *lparam = (struct vsfos_busybox_help_t *)ctx->user_buff;

	vsfsm_pt_begin(pt);

	lparam->handler = vsfos->shell.handlers;

	while ((lparam->handler != NULL) && (lparam->handler->name != NULL))
	{
		vsfshell_printf(outpt, "%s" VSFSHELL_LINEEND, lparam->handler->name);
		lparam->handler = lparam->handler->next;
	}

	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_uname(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "%s" VSFSHELL_LINEEND, vsfos->hwcfg->board);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_free(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_top(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// module handlers
static vsf_err_t vsfos_busybox_lsmod(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;
	struct vsfos_ctx_t *ctx = (struct vsfos_ctx_t *)param->context;
	struct vsfos_busybox_lsmod_t
	{
		struct vsf_module_t *module;
	} *lparam = (struct vsfos_busybox_lsmod_t *)ctx->user_buff;

	vsfsm_pt_begin(pt);

	lparam->module = vsf_module_get(NULL);

	while (lparam->module != NULL)
	{
		vsfshell_printf(outpt, "%08X: %s" VSFSHELL_LINEEND,
				(uint32_t)lparam->module->flash, lparam->module->flash->name);
		lparam->module = lparam->module->next;
	}

	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_repo(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// fs handlers
static char* vsfos_busybox_filetype(struct vsfile_t *file)
{
	if (file->attr & VSFILE_ATTR_VOLUMID)
		return "VOL";
	else if (file->attr & VSFILE_ATTR_DIRECTORY)
		return "DIR";
	else
		return "FIL";
}
static vsf_err_t vsfos_busybox_ls(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;
	struct vsfos_ctx_t *ctx = (struct vsfos_ctx_t *)param->context;
	struct vsfos_busybox_ls_t
	{
		struct vsfile_t *file;
		struct vsfsm_pt_t local_pt;
	} *lparam = (struct vsfos_busybox_ls_t *)ctx->user_buff;
	vsf_err_t err;

	vsfsm_pt_begin(pt);

	lparam->local_pt.state = 0;
	vsfsm_pt_entry(pt);
	err = vsfile_findfirst(&lparam->local_pt, evt, ctx->curfile, &lparam->file);
	if (err > 0) return err; else if (err < 0) goto end;

	while (lparam->file)
	{
		vsfshell_printf(outpt, "%s(%lld):%s" VSFSHELL_LINEEND,
				vsfos_busybox_filetype(lparam->file),
				lparam->file->size, lparam->file->name);

		// close file
		lparam->local_pt.state = 0;
		vsfsm_pt_entry(pt);
		err = vsfile_close(&lparam->local_pt, evt, lparam->file);
		if (err > 0) return err; else if (err < 0) goto srch_end;

		lparam->local_pt.state = 0;
		vsfsm_pt_entry(pt);
		err = vsfile_findnext(&lparam->local_pt, evt, ctx->curfile, &lparam->file);
		if (err > 0) return err; else if (err < 0) goto srch_end;
	}

srch_end:
	lparam->local_pt.state = 0;
	vsfsm_pt_entry(pt);
	err = vsfile_findend(&lparam->local_pt, evt, ctx->curfile);
	if (err > 0) return err; else if (err < 0) goto end;
end:
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_cd(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;
	struct vsfos_ctx_t *ctx = (struct vsfos_ctx_t *)param->context;
	struct vsfos_busybox_cd_t
	{
		struct vsfile_t *file;
		struct vsfsm_pt_t local_pt;
	} *lparam = (struct vsfos_busybox_cd_t *)ctx->user_buff;
	vsf_err_t err;

	vsfsm_pt_begin(pt);

	if (param->argc != 2)
	{
		vsfshell_printf(outpt, "format: %s PATH" VSFSHELL_LINEEND,
							param->argv[0]);
		goto end;
	}

	if (!strcmp(param->argv[1], "."))
	{
		goto end;
	}
	else if (!strcmp(param->argv[1], ".."))
	{
		if (ctx->curfile == (struct vsfile_t *)&vsfile.rootfs)
		{
			goto end;
		}

		vsfshell_printf(outpt, "not support" VSFSHELL_LINEEND);
		goto end;
	}
	else if (!strcmp(param->argv[1], "/"))
	{
		lparam->file = (struct vsfile_t *)&vsfile.rootfs;
	}
	else
	{
		lparam->local_pt.state = 0;
		vsfsm_pt_entry(pt);
		err = vsfile_getfile(&lparam->local_pt, evt, ctx->curfile,
								param->argv[1], &lparam->file);
		if (err > 0) return err; else if (err < 0)
		{
			vsfshell_printf(outpt, "file not found: %s" VSFSHELL_LINEEND,
								param->argv[1]);
			goto end;
		}
	}

	if (!(lparam->file->attr & VSFILE_ATTR_DIRECTORY))
	{
		vsfshell_printf(outpt, "%s is not a directory" VSFSHELL_LINEEND,
								param->argv[1]);
		goto end;
	}

	// close original curfile
	lparam->local_pt.state = 0;
	vsfsm_pt_entry(pt);
	err = vsfile_close(&lparam->local_pt, evt, ctx->curfile);
	if (err > 0) return err; else if (err < 0) goto end;

	ctx->curfile = lparam->file;
end:
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_pwd(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;
	struct vsfos_ctx_t *ctx = (struct vsfos_ctx_t *)param->context;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "%s" VSFSHELL_LINEEND, ctx->curfile->name);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_mkdir(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_rmdir(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_rm(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_mv(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_cp(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_cat(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// net handlers
static vsf_err_t vsfos_busybox_ipconfig(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_httpd(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

static vsf_err_t vsfos_busybox_dns(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// usb host
static vsf_err_t vsfos_busybox_lsusb(struct vsfsm_pt_t *pt, vsfsm_evt_t evt)
{
	struct vsfshell_handler_param_t *param =
						(struct vsfshell_handler_param_t *)pt->user_data;
	struct vsfsm_pt_t *outpt = &param->output_pt;

	vsfsm_pt_begin(pt);
	vsfshell_printf(outpt, "not supported now" VSFSHELL_LINEEND);
	vsfshell_handler_exit(pt);
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

vsf_err_t vsfos_busybox_init(struct vsfshell_t *shell)
{
	struct vsfshell_handler_t *handlers = vsf_bufmgr_malloc(VSFOSCFG_HANDLER_NUM * sizeof(*handlers));
	struct vsfos_ctx_t *ctx = vsf_bufmgr_malloc(sizeof(*ctx));
	int idx = 0;

	if (!handlers || !ctx) return VSFERR_NOT_ENOUGH_RESOURCES;
	memset(handlers, 0, VSFOSCFG_HANDLER_NUM * sizeof(struct vsfshell_handler_t));
	memset(ctx, 0, sizeof(*ctx));
	ctx->curfile = (struct vsfile_t *)&vsfile.rootfs;

	handlers[idx++] = (struct vsfshell_handler_t){"help", vsfos_busybox_help, ctx};
	handlers[idx++] = (struct vsfshell_handler_t){"uname", vsfos_busybox_uname, ctx};
	handlers[idx++] = (struct vsfshell_handler_t){"free", vsfos_busybox_free, ctx};
	handlers[idx++] = (struct vsfshell_handler_t){"top", vsfos_busybox_top, ctx};

	// module handlers
	handlers[idx++] = (struct vsfshell_handler_t){"lsmod", vsfos_busybox_lsmod, ctx};
	handlers[idx++] = (struct vsfshell_handler_t){"repo", vsfos_busybox_repo, ctx};

	// fs handlers
	if (vsf_module_get(VSFILE_MODNAME) != NULL)
	{
		handlers[idx++] = (struct vsfshell_handler_t){"ls", vsfos_busybox_ls, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"cd", vsfos_busybox_cd, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"pwd", vsfos_busybox_pwd, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"mkdir", vsfos_busybox_mkdir, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"rmdir", vsfos_busybox_rmdir, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"rm", vsfos_busybox_rm, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"mv", vsfos_busybox_mv, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"cp", vsfos_busybox_cp, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"cat", vsfos_busybox_cat, ctx};
	}

	// net handlers
	if (vsf_module_get(VSFIP_MODNAME) != NULL)
	{
		handlers[idx++] = (struct vsfshell_handler_t){"ipconfig", vsfos_busybox_ipconfig, ctx};
		handlers[idx++] = (struct vsfshell_handler_t){"ping", vsfos_busybox_ipconfig, ctx};
		if (vsf_module_get(VSFIP_HTTPD_MODNAME) != NULL)
		{
			handlers[idx++] = (struct vsfshell_handler_t){"httpd", vsfos_busybox_httpd, ctx};
		}
		if (vsf_module_get(VSFIP_DNSC_MODNAME) != NULL)
		{
			handlers[idx++] = (struct vsfshell_handler_t){"dns", vsfos_busybox_dns, ctx};
		}
	}

	// usb host
	if (vsf_module_get(VSFUSBH_MODNAME) != NULL)
	{
		handlers[idx++] = (struct vsfshell_handler_t){"lsusb", vsfos_busybox_lsusb, ctx};
	}

	handlers[idx++] = (struct vsfshell_handler_t)VSFSHELL_HANDLER_NONE;
	if (idx >= VSFOSCFG_HANDLER_NUM)
	{
		// memory destroyed
		while(1);
	}
	vsfshell_register_handlers(shell, handlers);
	return VSFERR_NONE;
}
