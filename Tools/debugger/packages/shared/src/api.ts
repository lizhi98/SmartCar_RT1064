import { type Method } from 'axios'

import { z } from 'zod'
import { Dataset, Empty, Failure, Result, Session, SessionId, Success } from '.'

interface ApiDefinition {
  method: Method
  req: z.ZodType
  res: z.ZodType
}

export const CreateSessionReq = z.object({
  datasetName: z.string(),
})
export type CreateSessionReq = z.infer<typeof CreateSessionReq>
export const CreateSessionRes = Session
export type CreateSessionRes = z.infer<typeof CreateSessionRes>

export const DeleteSessionReq = SessionId
export type DeleteSessionReq = z.infer<typeof DeleteSessionReq>
export const DeleteSessionRes = Result(z.undefined(), z.never())
export type DeleteSessionRes = z.infer<typeof DeleteSessionRes>

export const DatasetListReq = Empty
export type DatasetListReq = z.infer<typeof DatasetListReq>
export const DatasetListRes = z.array(Dataset)
export type DatasetListRes = z.infer<typeof DatasetListRes>

export const RunFrameReq = z.intersection(SessionId, z.object({
  frameIndex: z.number().optional(),
}))
export type RunFrameReq = z.infer<typeof RunFrameReq>
export const RunFrameRes = Result(
  z.object({
    stdout: z.string(),
    stderr: z.string(),
  }),
  z.string(),
)
export type RunFrameRes = z.infer<typeof RunFrameRes>

export const CompileReq = Empty
export type CompileReq = z.infer<typeof CompileReq>
export const CompileRes = Result(
  z.undefined(),
  z.string(),
)
export type CompileRes = z.infer<typeof CompileRes>

export const Api = {
  '/create-session': {
    method: 'POST',
    req: CreateSessionReq,
    res: CreateSessionRes,
  },
  '/delete-session': {
    method: 'POST',
    req: DeleteSessionReq,
    res: DeleteSessionRes,
  },
  '/dataset-list': {
    method: 'GET',
    req: DatasetListReq,
    res: DatasetListRes,
  },
  '/run-frame': {
    method: 'POST',
    req: RunFrameReq,
    res: RunFrameRes,
  },
  '/compile': {
    method: 'POST',
    req: CompileReq,
    res: CompileRes,
  },
} satisfies Record<string, ApiDefinition>

export type Api = typeof Api
