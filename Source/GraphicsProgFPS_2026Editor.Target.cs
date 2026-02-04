// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GraphicsProgFPS_2026EditorTarget : TargetRules
{
	public GraphicsProgFPS_2026EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "GraphicsProgFPS_2026" } );
	}
}
